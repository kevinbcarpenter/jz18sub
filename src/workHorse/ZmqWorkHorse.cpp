#include <string>
#include <thread>
#include <mutex>
using namespace std;

#include "ZmqWorkHorse.h"
#include "Logger.h"

mutex mtx;
ZmqWorkHorse* ZmqWorkHorse::workHorse{nullptr};
const string DUMP_IN{"fromClient.bin"}, DUMP_OUT{"toClient.bin"};

//---------------------------------------------------------------------------
ZmqWorkHorse::ZmqWorkHorse( const json& config, zmq::context_t& context ) : context( context ), socket( context, ZMQ_ROUTER ) {
	string connection{config[ "server" ][ "ip" ].get< string >( ) + ":" + to_string( config[ "server" ][ "zmqPort" ].get< ushort >( ) )};
	socket.bind( "tcp://" + connection );
	logger = Logger::getLogger( );

	logger->save( logInfo( __func__ ), "Listening on " + connection, IS_MAIN );
}

//---------------------------------------------------------------------------
void ZmqWorkHorse::createInstance( const json& config, zmq::context_t& context ) {
	if ( !workHorse ) {
		workHorse = new ZmqWorkHorse( config, context );
	}
}

//---------------------------------------------------------------------------
ZmqWorkHorse* ZmqWorkHorse::getInstance( ) {
	return workHorse;
}

//---------------------------------------------------------------------------
void ZmqWorkHorse::respond( const string& message, zmq::message_t& identity, int flags ) {
	// Dump
	logger->dump( DUMP_OUT, message );

	// Preserve our original identity
	zmq::message_t idCopy;
	idCopy.copy( &identity );

	// Lock and Send
	lock_guard< mutex > lck( mtx );
	zmq::message_t zMsg( message.data( ), message.size( ) );
	socket.send( idCopy, ZMQ_SNDMORE );
	socket.send( zMsg, flags );
}

//---------------------------------------------------------------------------
void ZmqWorkHorse::run( ) {
	logger->save( logInfo( __func__ ), "Listening", IS_MAIN );
	zmq::pollitem_t items[] = {{socket, 0, ZMQ_POLLIN, 0}};
	while ( true ) {
		try {
			zmq::poll( items, 1, 1 );
			if ( items[ 0 ].revents & ZMQ_POLLIN ) {
				logger->save( logInfo( __func__ ), "Connection received", IS_MAIN );

				// Grab ID and Message
				zmq::message_t identity, received;
				socket.recv( &identity );
				socket.recv( &received );

				// Parse message
				string message( static_cast< char* >( received.data( ) ), received.size( ) );
				json jMessage = json::parse( message );

				// Dump
				logger->dump( DUMP_IN, message );

				// Add to dbQueue
				// SpExecutor sPxtr( move( jMessage ), identity );
				// qmngr.addToQueue( sPxtr );
			}
		} catch ( exception& ex ) {
			logger->error( logInfo( __func__ ), ex.what( ), IS_MAIN );
		}
	}
}
