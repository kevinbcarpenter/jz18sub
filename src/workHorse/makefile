# Sources
MAIN_FILES = WorkHorse ZmqWorkHorse
LIB_FILES = App AppResources AppState Logger StateSub StringUtils

# Directory Includes
LIBRARY_DIR = $(subst $(notdir ${CURDIR}),,${CURDIR})include
THIRDPARTY_DIR =  $(subst $(notdir ${CURDIR}),,${CURDIR})include/thirdparty
INCLUDE_DIRS += -I$(LIBRARY_DIR)

# File Naming
MAIN_OBJS = $(addsuffix .o,$(MAIN_FILES))
SOURCES = $(MAIN_OBJS)
HEADERS = $(addsuffix .h,$(MAIN_FILES))
HEADERS += $(addsuffix .h,$(HEADER_ONLY))
LIB_OBJS = $(addsuffix .o,$(LIB_FILES))
SOURCES += $(LIB_OBJS)
HEADERS += $(addprefix $(LIBRARY_DIR)/,$(addsuffix .h,$(LIB_FILES)))

# Other sources
OTHER_SOURCES = json.hpp
SOURCES += $(addprefix $(THIRDPARTY_DIR)/,$(OTHER_SOURCES))
INCLUDE_DIRS += -I$(THIRDPARTY_DIR)

FRAMEWORK_SUPPORT = -DSTATE_SUPPORT

# Compiler and linker settings
CXX = g++ --std=c++14 -D_GLIBCXX_USE_CXX11_ABI=0
CXXFLAGS = -Wall -Wno-unknown-pragmas -Wno-format -O3 -Wno-deprecated

# Turns on debugging and allows backtrace symbols
CXXDEBUG = -g -rdynamic

default: workHorse

$(MAIN_OBJS): %.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) $(INCLUDE_DIRS) \
		$(FRAMEWORK_SUPPORT) -c $< -o $@

$(LIB_OBJS): %.o: $(LIBRARY_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) $(INCLUDE_DIRS) \
		$(FRAMEWORK_SUPPORT) -c $< -o $@
		
workHorse:
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) $(FRAMEWORK_SUPPORT) -o workHorse $(LDFLAGS) \
		$(SOURCES) \
		$(INCLUDE_DIRS) \
		-lpthread \
		-lzmq

workHorse: $(SOURCES) $(HEADERS) $(LIBSOURCES) makefile

clean:
	rm -f *.o *.out workHorse
