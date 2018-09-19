#ifndef WORKHORSE_CLASS
#define WORKHORSE_class

#include "App.h"

class WorkHorse : public App {
private:
	static std::string logInfo( std::string function ) {
		return "WorkHorse:" + function;
	}
	void setup( const json& config );

	bool installSegIntHandler( ) {
		return true;
	}

public:
	WorkHorse( std::string appName );
	~WorkHorse( );

	void run( );
};

#endif