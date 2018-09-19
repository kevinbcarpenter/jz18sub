# Sources
MAIN_FILES = MCP AppConnection Cortex ZConnection
LIB_FILES = App AppResources Logger StringUtils

# Directory Includes
LIBRARY_DIR = $(subst $(notdir ${CURDIR}),,${CURDIR})include
THIRDPARTY_DIR =  $(subst $(notdir ${CURDIR}),,${CURDIR})include/thirdparty
INCLUDE_DIRS += -I$(LIBRARY_DIR)

# File Naming
MAIN_OBJS = $(addsuffix .o,$(MAIN_FILES))
SOURCES = $(MAIN_OBJS)
HEADERS = $(addsuffix .h,$(MAIN_FILES))
LIB_OBJS = $(addsuffix .o,$(LIB_FILES))
LIB_HEADERS = $(addprefix $(LIBRARY_DIR)/,$(addsuffix .h,$(LIB_FILES)))
HEADERS += $(LIB_HEADERS)
SOURCES += $(LIB_OBJS)

# Other sources
OTHER_SOURCES = json.hpp
SOURCES += $(addprefix $(THIRDPARTY_DIR)/,$(OTHER_SOURCES))
INCLUDE_DIRS += -I$(THIRDPARTY_DIR)

# Compiler
CXX = g++ -D_GLIBCXX_USE_CXX11_ABI=0
CXXFLAGS = -Wall -Wno-unknown-pragmas -Wno-format -O3 -Wno-deprecated -Wno-non-virtual-dtor -Wtype-limits

# Turns on debugging and allows backtrace symbols
CXXDEBUG = -g -rdynamic

LDFLAGS = -static-libstdc++

default: mcp

$(MAIN_OBJS): %.o: %.cpp $(HEADERS) makefile
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) $(INCLUDE_DIRS) \
		$(FRAMEWORK_SUPPORT) -c $< -o $@

$(LIB_OBJS): %.o: $(LIBRARY_DIR)/%.cpp $(LIB_HEADERS) makefile
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) $(INCLUDE_DIRS) \
		$(FRAMEWORK_SUPPORT) -c $< -o $@

mcp:
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -o mcp $(LDFLAGS) \
		$(SOURCES) \
		-lpthread \
		-lzmq

mcp: $(SOURCES) makefile

clean:
	rm -f *.o *.out mcp

