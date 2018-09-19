#ifndef ZMQMCP_class
#define ZMQMCP_class

#include "ZConnection.h"

class AppConnection : public ZConnection {
private:
	static string logInfo( string function ) {
		return "AppConnection:" + function;
	}

	void handleReceivedMessage( string message );
	void zmqPoll( zmq::pollitem_t* items );

public:
	AppConnection( const json& config, zmq::context_t& context );
	void run( );
};

#endif