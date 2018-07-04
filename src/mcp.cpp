#include <iostream>
#include <algorithm>

#include <include/zmq.hpp>
#include "mcp.h"

using namespace std;

MasterControlProgram::MasterControlProgram( ) {
	;
}

MasterControlProgram::~MasterControlProgram( ) {
	;
}

void MasterControlProgram::run( ) {
	IsoCortex::createInstance( getContext( ) );
	JobSearch::createInstance( getContext( ) );

	AppConnection( getContext( ) ).run( );
}