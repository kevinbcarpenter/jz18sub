using namespace std;

#include "WorkHorse.h"
#include "Logger.h"
#include "zmq.hpp"
#include "ZbSocket.h"
#include "ZmqWorkHorse.h"

// Global constants
const string VERSION{"1.0.0"};
const string APP_NAME{"workHorse"};

//---------------------------------------------------------------------------
WorkHorse::WorkHorse( string appName ) : App( appName ) {
	stateLevel = StateLevel::RESOURCE;
}

//---------------------------------------------------------------------------
WorkHorse::~WorkHorse( ) {
	;
}

//---------------------------------------------------------------------------
void WorkHorse::run( ) {
	// Run setup
	App::run( );

	// Start server
	ZmqWorkHorse::createInstance( getConfig( ), getContext( ) );
	ZmqWorkHorse::getInstance( )->run( );
}

//---------------------------------------------------------------------------
void WorkHorse::setup( const json& config ) {
	try {
		// Complete setup
		logger->save( logInfo( __func__ ), "Version: " + VERSION, IS_MAIN );
	} catch ( exception& ex ) {
		logger->error( logInfo( __func__ ), string( "Setup failed: " ) + ex.what( ), IS_MAIN );
		exit( 1 );
	}
}

//---------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
	App::main( argc, argv, VERSION, new WorkHorse( APP_NAME ) );
}
