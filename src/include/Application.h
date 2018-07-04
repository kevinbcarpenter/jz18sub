#ifndef Application_Class
#define Application_Class

#include "zmq.hpp"
#include "json.hpp"
using json = nlohmann::json;

//----------------------------------------------------------------------------
// Resource defintions
//----------------------------------------------------------------------------
enum StateLevel { MCP, RESOURCE, MAX_LEVEL };
enum class State { disconnected, error, idle, paused, unkown, processing };
enum class Action { error, pause, pulse, progress, refresh, result, resume, state, task, terminate, warning };

enum class AppType { masterControl, dataStore };

//----------------------------------------------------------------------------
// Application State
//----------------------------------------------------------------------------
class AppState {
private:
	json js;
	State state;
	static AppState* astate;

	zmq::context_t& context;
	zmq::socket_t socket;

	AppState( zmq::context_t& context, const string& name );
	void pulse( );

public:
	AppState( ) = delete;
	AppState( AppState&& ) = delete;
	AppState( const AppState& ) = delete;
	void operator=( AppState&& ) = delete;
	void operator=( const AppState& ) = delete;

	static void makeInstance( zmq::context_t& context, const string& name );
	static AppState* getInstance( );

	void send( Action action, const json& jMessage );
	void send( Action action, const string& message = "" );

	void setState( State state );
	State getState( ) const;
	bool isRunning( ) const;

	void newTask( unsigned taskId, const string& taskName );
	void removeTask( unsigned taskId );
};

class Application {
private:
	zmq::context_t context;
	string _name;

	void setupStateMachine( );
	void setupStateSubscription( );

protected:
	ApplicationState* appState;
	StateSubscription* stateSub;
	StateLevel stateLevel{StateLevel::MCP};

public:
	Application( string name );
	virtual ~Application( );
	virtual void run( );

	static void main( int argc, char* argv[], const string& version, Application* app );

	void dumpOutput( string out );

	zmq::context_t getContext( );
}

#endif