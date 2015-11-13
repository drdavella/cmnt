CXX = /usr/bin/g++
TESTNAME = list_test
CXXFLAGS += -std=gnu++11 -Wall -Werror
CXXFLAGS += -I../../../include
LDFLAGS += -L.. -lcmnt_lister

.PHONY: all clean

all: ${TESTNAME}

%::%.cpp
	@@ echo
	@@ echo BUILDING TEST EXECUTABLE $@
	@@ echo ----------------------------------------
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDFLAGS}

clean:
	@@ echo
	@@ echo CLEANING TEST DIRECTORY
	@@ echo -----------------------
	rm -f ${TESTNAME}
