#ifndef MCP_CLASS
#define MCP_CLASS

#include <string>
#include <vector>
#include <future>

#include "thirdparty/zmq.hpp"
#include "thirdparty/json.hpp"
using json = nlohmann::json;

#include "include/AppResources.h"

class MCP {
private:
	zmq::context_t context;
	std::string appName;
	json configuration;
	json readConfiguration;

	static std::string logInfo( const std::string& function ) {
		return "App: " + function;
	}

	void configureStateMachine( );
	void configureStateSubscription( );

	AppState* appState{nullptr};
	StateSub* stateSub{nullptr};
	StateLevel stateLevel{StateLevel::MCP};

	void setup( const& json& configuration );

public:
	MCP( );
	~MCP( );

	void run( );

	zmq::context_t& getContext( );
};

#endif