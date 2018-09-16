#include <iostream>
#include <algorithm>

#include <zmq.hpp>
#include "mcp.h"

using namespace std;

MCP::MCP( ) {
	;
}

MCP::~MCP( ) {
	;
}

void MCP::run( ) {
	IsoCortex::createInstance( getContext( ) );
	JobSearch::createInstance( getContext( ) );

	AppConnection( getContext( ) ).run( );
}