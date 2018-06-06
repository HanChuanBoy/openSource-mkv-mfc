# libebml Makefile
# $Id: Makefile,v 1.20 2003/06/13 18:54:48 mosu Exp $
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
includedir=$(prefix)/include/ebml

# Programs
CXX=g++-4.8
LD=g++-4.8
DEP=$(CXX) -MM
DEPEND = makedepend
AR = ar rcvu
RANLIB = ranlib
INSTALL = install
INSTALL_OPTS = -m 644
INSTALL_OPTS_LIB = -m 644
INSTALL_DIR_OPTS = -m 755

# Options
EXTENSION=.cpp

ifeq (yes,$(DEBUG))
DEBUGFLAGS=-g -DDEBUG
endif
CXXFLAGS=$(DEBUGFLAGS) -Wall -Wno-unknown-pragmas -ansi -pedantic -fno-gnu-keywords -D_GNU_SOURCE

CWD=$(shell pwd)

SRC_DIR=$(CWD)/

# Librarires
INCLUDE=-I${SRC_DIR}
LIBS=

# Names
LIBRARY=libebml.a

# source-files
sources:=$(wildcard ${SRC_DIR}*$(EXTENSION))

# header files; replace .cxx extension with .h
headers:=$(patsubst %$(EXTENSION),%.h,$(sources))

# files holding dependency information; replace .cxx extension with .dep
dependencies:=$(patsubst %$(EXTENSION),%.dep,$(sources))

# object files; replace .cxx extension with .o
objects:=$(patsubst %$(EXTENSION),%.o,$(sources))

DEPENDFLAGS  = ${CXXFLAGS} ${INCLUDE}

all: $(LIBRARY)

lib library: $(LIBRARY)

# Build rules
%.o: %$(EXTENSION)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

$(LIBRARY): $(objects)
	$(AR) $@ $(objects)
	$(RANLIB) $@
#	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
#	${LD} -o $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}

clean:
	rm -f $(objects)
	rm -f $(dependencies)
	rm -f $(LIBRARY)
	rm -f CORE

# what are the source dependencies
depend: $(sources)
	$(DEPEND) $(DEPENDFLAGS) $(sources)

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
