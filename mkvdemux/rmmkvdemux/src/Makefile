APPLICATIONS = mkvextract

all: $(APPLICATIONS)
CXXFLAGS = -g -I./common -I./extract -I. -I../../include -I../include 
RUNAR = $(AR) rcu
RANLIB = ranlib
LINK = $(CXX) $(LDFLAGS) $(LIBDIRS) $(SYSTEM_LIBDIRS)
LINKSHARED = $(CXX) $(LDFLAGS) $(LIBDIRS) $(SYSTEM_LIBDIRS) \
		-shared -Wl,--export-all
CXXCOMPILE = $(CXX) $(CXXFLAGS) $(INCLUDES) $(SYSTEM_INCLUDES)
CCOMPILE = $(CC) $(CFLAGS) $(INCLUDES) $(SYSTEM_INCLUDES)

%.o: %.cpp
	@echo '     CXX ' $<
	$(CXXCOMPILE) -c -o $@ $<

#
# common
#

libmtxcommon_SOURCES = $(wildcard common/*.cpp)
libmtxcommon_OBJECTS := $(patsubst %.cpp,%.o,$(libmtxcommon_SOURCES))

common/libmtxcommon.a: $(libmtxcommon_OBJECTS)
	@rm -f $@
	@echo '      AR ' $@
	$(RUNAR) $@ $(libmtxcommon_OBJECTS)
	@echo '  RANLIB ' $@
	$(RANLIB) $@

#
# extract
#
libmtxextract_SOURCES = $(wildcard extract/*.cpp)
libmtxextract_OBJECTS := $(patsubst %.cpp,%.o,$(libmtxextract_SOURCES))

extract/libmtxextract.a: $(libmtxextract_OBJECTS)
	@rm -f $@
	@echo '      AR ' $@
	$(RUNAR) $@ $(libmtxextract_OBJECTS)
	@echo '  RANLIB ' $@
	$(RANLIB) $@

ICONV_LIBS := -liconv
mkvextract_SOURCES := mkvextract.cpp
mkvextract_OBJECTS := $(patsubst %.cpp,%.o,$(mkvextract_SOURCES))
mkvextract_DEPENDENCIES += ./common/libmtxcommon.a ./extract/libmtxextract.a 
mkvextract_LDADD = -L./extract -lmtxextract -L/usr/local/libiconv/lib -L./common -lmtxcommon -lmatroska -lebml -lpthread \
		$(ICONV_LIBS) $(LIBINTL_LIBS) -lexpat \
		-lz $(COMPRESSION_LIBRARIES) $(LIBRPCRT)

mkvextract: $(mkvextract_OBJECTS) $(mkvextract_DEPENDENCIES)
	@echo '    LINK ' $@
	$(LINK) -o $@ $(mkvextract_OBJECTS) $(mkvextract_LDADD)

clean:
	rm -f *.o */*.o */*/*.o */lib*.a */*/lib*.a po/*.mo mkvextract \
		*/*.exe */*/*.exe */*/*.dll */*/*.dll.a doc/*.hhk
