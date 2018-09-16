
SOURCES = mcp.cpp
HEADERS = mcp.h

LIB_SOURCES = application.cpp
LIB_HEADERS = application.h

# includes
LIBDIR = $(subst $(notdir ${CURDIR}),,${CURDIR})include
THIRDPARTY_DIR =  $(subst $(notdir ${CURDIR}),,${CURDIR})include/thirdparty
INCLUDE_DIRS += -I$(LIBRARY_DIR)

# Other sources
OTHER_SOURCES = json.hpp zmq.hpp
SOURCES += $(addprefix $(THIRDPARTY_DIR)/,$(OTHER_SOURCES))
INCLUDE_DIRS += -I$(THIRDPARTY_DIR)


CXX = g++ --std=c++14
CXXFLAGS = -Wall -Wno-unknown-pragmas -Wno-format -O3 -Wno-deprecated -Wtype-limits

# Turns on debugging and allows backtrace symbols
CXXDEBUG = -g -rdynamic

# Linker
LDFLAGS = -static-libstdc++

all: mcp dbtask

OTHER_OBJS = application.o

mcp: mcp.o $(OTHER_OJBS)

dbtask: dbtask.o ($OTHER_OBJS)