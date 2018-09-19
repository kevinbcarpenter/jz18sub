#ifndef ZCONNECTION_class
#define ZCONNECTION_class

#include <map>

#include "AppResources.h"
#include "Cortex.h"
#include "Logger.h"
#include "zmq.hpp"
#include "json.hpp"
using json = nlohmann::json;

class ZConnection {
private:
	static string logInfo( string function ) {
		return "ZConnection:" + function;
	}

protected:
	zmq::context_t& context;
	zmq::socket_t socket;

	Cortex* cortex{nullptr};
	Logger* logger{nullptr};

public:
	ZConnection( zmq::context_t& context );
};

#endif
