#ifndef CORTEX_class
#define CORTEX_class

#include <unordered_map>

#include "AppResources.h"
#include "Logger.h"

#include "zmq.hpp"
#include "json.hpp"
using json = nlohmann::json;

typedef map< u_long, string > JobList;
typedef unordered_map< AppIdentity, AppResource > AppMap;
typedef unordered_map< AppIdentity, JobList > JobMap;

class Cortex {
private:
	const json& config;
	zmq::context_t& context;

	unsigned monitorTime;
	double downTime, removeTime;
	Logger* logger{nullptr};

	AppIdentity ctxId;

	AppMap appMap;

	static Cortex* cortex;
	Cortex( const json& config, zmq::context_t& context );

	static string logInfo( string function ) {
		return "Cortex:" + function;
	}

	/***********************************************************************
	** Maintains appMap's integrity
	**
	** Return:	N/A
	***********************************************************************/
	void monitor( );

	/***********************************************************************
	** Daemon that forwards messages to all subscribed applications based
	** on the application's subscription level.
	**
	** Return:	N/A
	***********************************************************************/
	void publisher( );

	/***********************************************************************
	** Gets the local date/time
	**
	** Return:	tm struct containing date/time data
	***********************************************************************/
	tm getDateTime( ) const;

	/***********************************************************************
	** Updates the database with application info for all applications
	** currently running a job.
	**
	** Params:	itr - an iterator pointing to an AppMap pair
	**
	** Return:	N/A
	***********************************************************************/
	void updateAppStatusTable( const AppIdentity& app, const AppResource& res, bool update = false );

	/***********************************************************************
	** Sends a job message to the apporpriate application
	**
	** Params:	js - a json message containing a full job request
	**			app - An AppType
	**
	** Return:	N/A
	***********************************************************************/
	void sendJob( const json& js, AppType app ) const;

public:
	Cortex( ) = delete;
	Cortex( const Cortex& ) = delete;
	void operator=( const Cortex& ) = delete;

	static void createInstance( const json& config, zmq::context_t& context );
	static Cortex* getInstance( );

	/***********************************************************************
	 ** Initiates Cortex's connection to the database
	 **
	 ** Return:	N/A
	 ***********************************************************************/
	void connect( );

	/***********************************************************************
	** Called whenever a message sends a heartbeat. This function updates
	** appMap with the applications state and job list.
	**
	** Return:	N/A
	***********************************************************************/
	void heartbeat( const json& js );

	/***********************************************************************
	** Sends an Action message to an application
	**
	** Params:	js - a json message containing an AppIdentity as well as an
	**				action for an application.
	**
	** Return:	N/A
	***********************************************************************/
	void sendAction( const json& js ) const;

	/***********************************************************************
	** Sends multiple broadcast messages of all application states to
	** any subscribed application.
	**
	** Return:	N/A
	***********************************************************************/
	void broadcastAll( ) const;

	/***********************************************************************
	** Sends a broadcast message to any subscribed application.
	**
	** Return:	N/A
	***********************************************************************/
	void broadcast( const string& str ) const;

	/***********************************************************************
	** Dumps the contents of appMap and jobMap to console.
	**
	** Return:	N/A
	***********************************************************************/
	void dump( ) const;

	/***********************************************************************
	** When an application's state changes, this function is called to
	** modify the local appMap.
	**
	** Return:	N/A
	***********************************************************************/
	void setState( const json& js );

	/***********************************************************************
	** If an application has a task that needs to be handled by AutoDir,
	** this function forwards the message to the appropriate class.
	**
	** Return:	N/A
	***********************************************************************/
	void appTask( const json& js );

	/***********************************************************************
	** Finds the best connection of a specified AppType
	**
	** Return:	An AppIdentity of the connection.
	***********************************************************************/
	AppIdentity getConnection( const AppType& appType ) const;

	/***********************************************************************
	** Creates and returns a json object containing all application states
	** and running jobs. Used for communication with Web UI.
	***********************************************************************/
	json clientStatusList( );
};

#endif
