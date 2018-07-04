#ifndef MCP_CLASS
#define MCP_CLASS

#include <string>
#include <future>

#include "include/json.hpp"
using json = nlohmann::json;

class MCP {
private:
	std::string status;

public:
	MCP( );
	~MCP( );

	void run( );
};

#endif