#ifndef ZMQWORKHORSE_class
#define ZMQWORKHORSE_class

#include "zmq.hpp"
#include "json.hpp"
using json = nlohmann::json;

/***********************************************************************
** Forward Declarations
***********************************************************************/
class Logger;

/***********************************************************************
** Class Declaration
***********************************************************************/
class ZmqWorkHorse {
private:
	zmq::context_t& context;
	zmq::socket_t socket;

	static ZmqWorkHorse* workHorse;

	ZmqWorkHorse( const json& config, zmq::context_t& context );

	/***********************************************************************
	** Logger Stuff
	***********************************************************************/
	Logger* logger{nullptr};
	static std::string logInfo( std::string function ) {
		return "ZmqWorkHorse:" + function;
	}

public:
	/***********************************************************************
	** Singleton Functions
	***********************************************************************/
	static void createInstance( const json& config, zmq::context_t& context );
	static ZmqWorkHorse* getInstance( );

	/***********************************************************************
	** Sends a message via our ZMQ socket back to the calling app
	**
	** Params:	jMessage - the message that is returned
	**			identity - zmq identity used to return message to the
	**					   correct app
	**			flags - used for ZMQ_SNDMORE or other options (if needed)
	**
	** Return:	N/A
	***********************************************************************/
	void respond( const std::string& jMessage, zmq::message_t& identity, int flags = 0 );

	/***********************************************************************
	** Runs the server
	**
	** Return:	N/A
	***********************************************************************/
	void run( );
};

#endif
