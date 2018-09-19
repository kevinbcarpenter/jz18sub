#include <thread>
#include <iostream>
#include <shared_mutex>

using namespace std;

#include "Cortex.h"
#include "EpxUtils.h"
#include "StringUtils.h"

using st_mtx = shared_timed_mutex;

mutex update_mtx;
st_mtx appMapMtx;
Cortex* Cortex::cortex{nullptr};

const short SEC_PER_MIN{60};
const string CORTEX_DUMP_FILE{"cortex.bin"};

//---------------------------------------------------------------------------
Cortex::Cortex( const json& config, zmq::context_t& context )
	: config( config ), context( context ), monitorTime( config[ "timers" ][ "monitorTime" ] ), downTime( config[ "timers" ][ "downTime" ] ), removeTime( config[ "timers" ][ "removeTime" ] ) {
	thread( &Cortex::monitor, this ).detach( );
	thread( &Cortex::publisher, this ).detach( );

	logger = Logger::getLogger( );

	this->ctxId = AppIdentity( AppType::mcp, config[ "server" ][ "ip" ].get< string >( ), config[ "server" ][ "appPort" ].get< ushort >( ) );
}

//---------------------------------------------------------------------------
void Cortex::createInstance( const json& config, zmq::context_t& context ) {
	if ( !cortex ) {
		cortex = new Cortex( config, context );
	}
}

//---------------------------------------------------------------------------
Cortex* Cortex::getInstance( ) {
	return cortex;
}

//---------------------------------------------------------------------------
void Cortex::monitor( ) {
	while ( true ) {
		try {
			this_thread::sleep_for( chrono::seconds( monitorTime ) );
			auto now{chrono::system_clock::to_time_t( chrono::system_clock::now( ) )};

			// Update appMap and app status table
			lock_guard< st_mtx > lck( appMapMtx );
			for ( auto app{appMap.begin( )}; app != appMap.end( ); ) {
				double difference{difftime( now, mktime( &app->second.lastChange ) ) / SEC_PER_MIN};
				if ( app->second.isAlive( ) ) {
					if ( difference >= downTime ) {
						logger->save( logInfo( __func__ ), app->first.getAppId( ) + " disconnected", IS_MAIN );
						app->second = AppResource{State::disconnected, getDateTime( )};
						broadcast( merge( app->first, app->second ).dump( ) );
					}
				} else if ( app->second.isDisconnected( ) ) {
					if ( difference >= removeTime && removeTime ) {
						// Remove from appMap
						string removed{app->first.getAppId( )};
						app = appMap.erase( app );
						logger->save( logInfo( __func__ ), removed + " removed", IS_MAIN );
						continue;
					}
				}
				++app;
			}
		} catch ( exception& ex ) {
			logger->error( logInfo( __func__ ), ex.what( ), IS_MAIN );
		} catch ( ... ) {
			logger->error( logInfo( __func__ ), "An unknown exception occurred", IS_MAIN );
		}

		broadcastAll( );
	}
}

//---------------------------------------------------------------------------
void Cortex::publisher( ) {
	zmq::socket_t inSock( context, ZMQ_PULL );
	zmq::socket_t publisher( context, ZMQ_PUB );
	inSock.bind( "inproc://publisher" );
	publisher.bind( "tcp://" + config.at( "server" ).at( "ip" ).get< string >( ) + ":" + to_string( config.at( "server" ).at( "pubPort" ).get< ushort >( ) ) );

	while ( true ) {
		try {
			zmq::message_t inMsg;
			inSock.recv( &inMsg );
			AppIdentity appId = json::parse( inMsg.data< char >( ), inMsg.data< char >( ) + inMsg.size( ) );

			ushort level{StateLevel::MAX_LEVEL};
			/***********************************************************************
			** Set level to only broadcast message to Apps that require an update
			** when state is changed. EX: If an HsmProxy sent a state change, when
			** the message is received by the publisher, appId.name will be
			** AppType::hsmProxy. Therefore, all apps with a state level of
			** RESOURCE or above will receive the message.
			**
			** AppTypes not in this list will only broadcast messages to apps
			** that require an update of every application.
			***********************************************************************/
			switch ( appId.name ) {
				case AppType::mcp:
					level = StateLevel::MCP;
					break;
				case AppType::workHorse:
					level = StateLevel::RESOURCE;
					break;
				default:
					break;
			}

			for ( ushort i{level}; i <= MAX_LEVEL; ++i ) {
				string tmp{to_string( i )};
				zmq::message_t addr( tmp.data( ), tmp.length( ) );
				zmq::message_t msg;
				msg.copy( &inMsg );

				publisher.send( addr, ZMQ_SNDMORE );
				publisher.send( msg );
			}

		} catch ( exception& ex ) {
			logger->warn( logInfo( __func__ ), ex.what( ), IS_MAIN );
		}
	}
}

//----------------------------------------------------------------------------
void Cortex::broadcastAll( ) const {
	zmq::socket_t inSock( context, ZMQ_PUSH );
	inSock.connect( "inproc://publisher" );
	shared_lock< st_mtx > lck( appMapMtx );
	for ( const auto& app : appMap ) {
		json id = app.first;
		json res = app.second;
		string str{merge( id, res ).dump( )};

		zmq::message_t message( str.data( ), str.length( ) );
		inSock.send( message );
	}
}

//----------------------------------------------------------------------------
void Cortex::broadcast( const string& str ) const {
	zmq::socket_t inSock( context, ZMQ_PUSH );
	zmq::message_t message( str.data( ), str.length( ) );

	inSock.connect( "inproc://publisher" );
	inSock.send( message );
}

//----------------------------------------------------------------------------
tm Cortex::getDateTime( ) const {
	auto tt{chrono::system_clock::to_time_t( chrono::system_clock::now( ) )};
	tm t{*localtime( &tt )};
	return t;
}

//---------------------------------------------------------------------------
void Cortex::connect( ) {
	;
}

//---------------------------------------------------------------------------
void Cortex::heartbeat( const json& js ) {
	AppIdentity id = js;
	unique_lock< st_mtx > lck( appMapMtx );
	auto newApp{appMap.find( id )};
	if ( newApp == appMap.end( ) ) {
		logger->save( logInfo( __func__ ), "New application connection: " + id.getAppId( ), IS_MAIN );
	} else if ( newApp->second.state == State::disconnected ) {
		logger->save( logInfo( __func__ ), id.getAppId( ) + " reconnected", IS_MAIN );
	}

	// Create Resource
	AppResource res{js.get< State >( ), getDateTime( )};
	appMap[ id ] = res;
	lck.unlock( );
}

//---------------------------------------------------------------------------
void Cortex::sendAction( const json& js ) const {
	// Build action
	AppIdentity id = js;
	{
		shared_lock< st_mtx > lck( appMapMtx );
		if ( appMap.find( id ) == appMap.end( ) ) {
			logger->warn( logInfo( __func__ ), "Invalid AppIdentity: " + id.getAppId( ), IS_MAIN );
			return;	// Not a valid app
		}
	}

	zmq::socket_t socket( context, ZMQ_DEALER );
	socket.connect( "tcp://" + id.getConnectionInfo( ) );

	string temp{js.dump( )};
	zmq::message_t message( temp.data( ), temp.length( ) );
	socket.send( message );

	logger->save( logInfo( __func__ ), "Action command sent to " + id.getAppId( ), IS_MAIN );
}

//---------------------------------------------------------------------------
AppIdentity Cortex::getConnection( const AppType& appType ) const {
	ushort retry{3};
	if ( appType == AppType::mcp ) {
		return ctxId;
	}

	while ( appMap.empty( ) ) {
		this_thread::sleep_for( chrono::seconds( 1 ) );
	}

	AppMap::const_iterator appRes;

	do {
		shared_lock< st_mtx > lck( appMapMtx );
		appRes = appMap.end( );

		// Find an idle or optimal resource
		for ( auto it{appMap.begin( )}; it != appMap.end( ); ++it ) {
			if ( it->first.name == appType ) {
				if ( it->second.state == State::idle ) {
					return it->first;
				} else {
					if ( appRes != appMap.end( ) ) {
						appRes = it->second.lastChange < appRes->second.lastChange ? it : appRes;
					} else {
						appRes = it;
					}
				}
			}
		}

		// If we can't find the app, sleep for a bit and try again
		if ( appRes == appMap.end( ) ) {
			lck.unlock( );
			if ( --retry ) {
				logger->warn( logInfo( __func__ ), "Failed to find " + toString( appType ) + " - Verify app is running", IS_MAIN );
				this_thread::sleep_for( chrono::seconds( monitorTime ) );
			} else {
				throw runtime_error( "Failed to find " + toString( appType ) + " - Skipping" );
			}
		} else {
			break;
		}
	} while ( true );

	return appRes->first;
}

//---------------------------------------------------------------------------
void Cortex::sendJob( const json& js, AppType app ) const {
	string msg{js.dump( )};

	zmq::socket_t socket( context, ZMQ_DEALER );
	socket.connect( "tcp://" + getConnection( app ).getConnectionInfo( ) );

	zmq::message_t message( msg.data( ), msg.length( ) );
	socket.send( message );
	logger->save( logInfo( __func__ ), "Job sent - " + js.dump( ), IS_MAIN );
}

//---------------------------------------------------------------------------
void Cortex::dump( ) const {
	ostringstream oss;
	shared_lock< st_mtx > lck( appMapMtx );
	logger->save( logInfo( __func__ ), "AppResources Count: ", appMap.size( ), IS_MAIN );
	oss << "AppResources Count: " << appMap.size( ) << endl;
	oss << left << setw( 20 ) << "APP_NAME" << setw( 20 ) << "IP" << setw( 20 ) << "PORT" << setw( 20 ) << "STATE" << setw( 24 ) << "LAST_CHANGE" << endl;
	for ( const auto& res : appMap ) {
		stringstream ss;
		ss << left << setw( 20 ) << res.first.name << setw( 20 ) << res.first.ip << setw( 20 ) << res.first.port << setw( 20 ) << res.second.state << setw( 24 ) << res.second.lastChange << endl;

		logger->dump( CORTEX_DUMP_FILE, ss.str( ) );
		oss << ss.str( );
	}
	oss << endl;
	cout << oss.str( );
}

//---------------------------------------------------------------------------
void Cortex::setState( const json& js ) {
	AppIdentity id = js;
	AppResource res{js.get< State >( ), getDateTime( )};

	{	// Lock and Update
		lock_guard< st_mtx > lck( appMapMtx );
		appMap[ id ] = res;
	}

	// Log Info
	if ( logger->dump( ) ) {
		ostringstream oss;
		oss << id.getAppId( ) << " state modified: " << res.state;
		logger->dump( CORTEX_DUMP_FILE, oss.str( ) );
	}

	// Broadcast and Update
	json jRes = res;
	broadcast( merge( js, jRes ).dump( ) );
}

//----------------------------------------------------------------------------
json Cortex::clientStatusList( ) {
	json result;

	shared_lock< st_mtx > lck( appMapMtx );
	for ( const auto& res : appMap ) {
		result[ "clientStatusList" ].push_back( merge( res.first, res.second ) );
	}
	lck.unlock( );

	return result;
}
