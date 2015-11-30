CC = /usr/bin/g++
LIBNAME = cmnt_lister
LIBFILE = lib${LIBNAME}.a
INSTALL = ../../lib

CXXFLAGS = -std=gnu++11 -Wall -Werror -O3
INCDIRS = -I. -I../../include
LIBDIRS = 

.PHONY: all install clean

all: ${LIBFILE}

%.o: %.cpp
	@@ echo
	@@ echo CREATING OBJECT FILE $@
	@@ echo ----------------------------------
	${CC} ${CXXFLAGS} ${INCDIRS} -c -o $@ $^

lib%.a: %.o
	@@ echo
	@@ echo PACKAGING STATIC LIBRARY $@
	@@ echo -----------------------------------------
	ar rcs $@ $^

install: ${LIBFILE}
	@@ echo
	@@ echo INSTALLING TO ${INSTALL}
	@@ echo -----------------------------------
	cp ${LIBFILE} ${INSTALL}

clean:
	@@ echo
	@@ echo CLEANING DIRECTORY
	@@ echo ------------------
	rm -f ${LIBFILE}
	rm -f *.o
