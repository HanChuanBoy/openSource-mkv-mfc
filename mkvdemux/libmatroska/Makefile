# libmatroska core Makefile (used in cygwin)
# $Id: Makefile,v 1.23 2003/06/13 18:53:15 mosu Exp $
# Author: Steve Lhomme <robux4 @ users.sf.net>
# Author: Moritz Bunkus <moritz @ bunkus.org>

#
# The library is built without debug information. If you want
# debug information to be included then compile with
# 'make DEBUG=yes'.
#

# Paths
prefix=/usr/local
libdir=$(prefix)/lib
includedir=$(prefix)/include/matroska

# Programs
CXX=g++
LD=g++
DEP=$(CXX) -MM
DEPEND = makedepend
AR = ar rcvu
RANLIB = ranlib
INSTALL = install
INSTALL_OPTS = -m 644
INSTALL_OPTS_LIB = -m 644
INSTALL_DIR_OPTS = -m 755

CWD=$(shell pwd)

# Options
LIBEBML_INCLUDE_DIR=$(CWD)/../libebml/src
LIBEBML_LIB_DIR=$(CWD)/../libebml/src
EXTENSION=.cpp

SYSTEM  := $(shell uname -s)
MACHINE := $(shell uname -m)
ifeq (CYGWIN,$(findstring CYGWIN,$(SYSTEM)))
	LIBICONV=-liconv
endif

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif
CXXFLAGS=$(DEBUGFLAGS) -Wall -Wno-unknown-pragmas -ansi -pedantic -fno-gnu-keywords -D_GNU_SOURCE
LDFLAGS=-L. -L$(LIBEBML_LIB_DIR)

SRC_DIR=$(CWD)/
MUX_SRC_DIR=$(CWD)/test/mux/

# Librarires
INCLUDE=-I${SRC_DIR} -I$(LIBEBML_INCLUDE_DIR)
LIBS=
MUX_LIBS=-lmatroska -lebml $(LIBICONV)

# Names
LIBRARY=libmatroska.a

# source-files
sources:=$(wildcard ${SRC_DIR}*$(EXTENSION))

# header files; replace .cxx extension with .h
headers:=$(patsubst %$(EXTENSION),%.h,$(sources))

# files holding dependency information; replace .cxx extension with .dep
dependencies:=$(patsubst %$(EXTENSION),%.dep,$(sources))

# object files; replace .cxx extension with .o
objects:=$(patsubst %$(EXTENSION),%.o,$(sources))

DEPENDFLAGS  = ${CXXFLAGS} ${INCLUDE}

all: $(LIBRARY) test

lib library: $(LIBRARY)

# Build rules
%.o: %$(EXTENSION)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

$(LIBRARY): $(objects)
	$(AR) $@ $(objects)
	$(RANLIB) $@
#	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
#	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

clean:	cleantest
	rm -f $(objects)
	rm -f $(dependencies)
	rm -f $(LIBRARY)
	rm -f CORE

cleantest:
	rm -f test6 test8 test6.o test8.o

# what are the source dependencies
depend: $(sources)
	$(DEPEND) $(DEPENDFLAGS) $(sources)

test: test6 test8

test6:	test6.o $(LIBRARY)
	$(LD) -o $@ $(LDFLAGS) $< $(MUX_LIBS)

test6.o: $(MUX_SRC_DIR)test6.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

test8:	test8.o $(LIBRARY)
	$(LD) -o $@ $(LDFLAGS) $< $(MUX_LIBS)

test8.o: $(MUX_SRC_DIR)test8.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

install: $(LIBRARY)
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(libdir)
	$(INSTALL) $(INSTALL_OPTS_LIB) $(LIBRARY) $(libdir)
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(includedir)
	for i in $(SRC_DIR)/*.h; do \
		$(INSTALL) $(INSTALL_OPTS) $$i $(includedir) ; \
	done
	$(INSTALL) $(INSTALL_DIR_OPTS) -d $(includedir)/api/c
	for i in $(SRC_DIR)/api/c/*.h; do \
		$(INSTALL) $(INSTALL_OPTS) $$i $(includedir)/api/c ; \
	done

# DO NOT DELETE
