#include <string>

using namespace std;

#include "ZConnection.h"
#include "AppResources.h"

//---------------------------------------------------------------------------
ZConnection::ZConnection( zmq::context_t& context ) : context( context ), socket( context, ZMQ_ROUTER ) {
	this->logger = Logger::getLogger( );
}
