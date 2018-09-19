#include <iostream>
#include <algorithm>
#include <zmq.hpp>
#include <args.hxx>

using namespace std;

#include "App.h"
#include "MCP.h"
#include "AppConnection.h"
#include "Cortex.h"

const string VERSION{"1.0.0"};
const string APP_NAME{"mcp"};
const string TITLE{APP_NAME + " - " + VERSION};

args::ArgumentParser parser( TITLE, "Tool for processing job automation." );
args::HelpFlag clHelp( parser, "help", "Displays this help menu.", {'h', "help"} );
args::ValueFlag< bool > clDumps{parser, "0 or 1", "Turn on dumps.", {'d', "dumps"}};

//---------------------------------------------------------------------------
MasterControlPrg::MasterControlPrg( string appName ) : App( appName ) {
	// Do nothing
}

//---------------------------------------------------------------------------
MasterControlPrg::~MasterControlPrg( ) {
	if ( logger ) {
		delete logger;
	}
}

//---------------------------------------------------------------------------
void MasterControlPrg::setup( const json& configuration ) {
	logger->setDump( clDumps ? args::get( clDumps ) : configuration[ "dumps" ][ "on" ].get< bool >( ) );

	// Log our startup information
	logger->save( logInfo( __func__ ), "Version: " + VERSION, IS_MAIN );
	logger->save( logInfo( __func__ ), "Listen IP: ", configuration[ "server" ][ "ip" ].get< string >( ), IS_MAIN );
	logger->save( logInfo( __func__ ), "App listen port: ", configuration[ "server" ][ "appPort" ].get< int >( ), IS_MAIN );
	logger->save( logInfo( __func__ ), "Dump packets is: " + ( Logger::dump( ) ? string( "on" ) : string( "off" ) ), IS_MAIN );
}

//---------------------------------------------------------------------------
void MasterControlPrg::run( ) {
	App::run( );

	// Setup singletons
	Cortex::createInstance( getConfig( ), getContext( ) );

	// Create listeners
	AppConnection( getConfig( ), getContext( ) ).run( );
}

//---------------------------------------------------------------------------
int main( int argc, char* argv[] ) {
	try {
		parser.ParseCLI( argc, argv );
	} catch ( args::Help ) {
		cout << parser;
		return 0;
	} catch ( args::ParseError& e ) {
		cerr << e.what( ) << endl;
		cerr << parser;
		return 1;
	} catch ( args::ValidationError& e ) {
		cerr << e.what( ) << endl;
		cerr << parser;
		return 1;
	}

	App::main( argc, argv, VERSION, new MasterControlPrg( APP_NAME ) );
}
