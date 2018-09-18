#include <iostream>
#include <algorithm>

#include "mcp.h"

using namespace std;

MCP::MCP( ) {
	;
}

MCP::~MCP( ) {
	;
}

json MCP::readConfiguration( ) {
	json jsonConfig;
	try {
		ifstream configFile( string( appName + ".json" ).c_str( ) );
		if ( configFile.ios::eof( ) ) {
			throw runtime_error( "Config file is empty" );
		}

		if ( !configFile.is_open( ) ) {
			throw runtime_error( "Can't open config" );
		}

		configFile >> jsonConfig;
	} catch ( exception& e ) {
		throw string( e.what( ) );
	} catch ( ... ) {
		throw "Can't open config";
	}
	return jsonConfig;
}

void MCP::run( ) {
}