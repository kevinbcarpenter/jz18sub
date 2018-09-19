#include <sstream>
#include <iostream>
#include <thread>

using namespace std;

#include "AppConnection.h"

const string IN_MESSAGE{"fromApps.bin"};

//---------------------------------------------------------------------------
AppConnection::AppConnection( const json& config, zmq::context_t& context ) : ZConnection( context ) {
	socket.bind( "tcp://" + config.at( "server" ).at( "ip" ).get< string >( ) + ":" + to_string( config.at( "server" ).at( "appPort" ).get< short >( ) ) );
	cortex = Cortex::getInstance( );
}

//---------------------------------------------------------------------------
void AppConnection::zmqPoll( zmq::pollitem_t* items ) {
	zmq::poll( items, 1, -1 );
	if ( items[ 0 ].revents & ZMQ_POLLIN ) {
		zmq::message_t identity;
		zmq::message_t received;

		// Receive message
		socket.recv( &identity );
		socket.recv( &received );

		string message( static_cast< char* >( received.data( ) ), received.size( ) );
		thread( &AppConnection::handleReceivedMessage, this, message ).detach( );
	}
}

//---------------------------------------------------------------------------
void AppConnection::handleReceivedMessage( string message ) {
	logger->dump( IN_MESSAGE, message );

	try {
		json jMessage = json::parse( message );
		switch ( jMessage.get< Action >( ) ) {
			case Action::progress:
				break;
			case Action::heartbeat:
				cortex->heartbeat( jMessage );
				break;
			case Action::pause:
			case Action::resume:
				cortex->sendAction( jMessage );
				break;
			case Action::dump:
				cortex->dump( );
				break;
			case Action::refresh:
				cortex->broadcastAll( );
				break;
			case Action::state:
				cortex->setState( jMessage );
				break;
			case Action::warn:
				break;
			default:
				logger->warn( logInfo( __func__ ), "Invalid Action", IS_THREAD );
				break;
		}
	} catch ( exception& ex ) {
		logger->error( logInfo( __func__ ), ex.what( ), IS_THREAD );
	}
}

//---------------------------------------------------------------------------
void AppConnection::run( ) {
	logger->save( logInfo( __func__ ), "Listening", IS_MAIN );
	zmq::pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
	while ( true ) {
		try {
			zmqPoll( items );
		} catch ( exception& ex ) {
			logger->error( logInfo( __func__ ), ex.what( ), IS_MAIN );
		} catch ( ... ) {
			logger->error( logInfo( __func__ ), "An unknown error occurred", IS_MAIN );
		}
	}
}
