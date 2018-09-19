#ifndef MCP_CLASS
#define MCP_CLASS

#include <future>

#include "json.hpp"
using json = nlohmann::json;

class MasterControlPrg : public App {
private:
	bool dumpResponse{false};

	static string logInfo( string function ) {
		return "MasterControlPrg:" + function;
	}

	void setup( const json& configuration );

	bool shouldStartConfigThread( ) {
		return false;
	}

	bool installSegIntHandler( ) {
		return true;
	}

public:
	MasterControlPrg( string appName );
	~MasterControlPrg( );

	void run( );
};

#endif