#------------------------------------------------------------------------------
#
#  You can set several environment variables before running make if you don't
#  like the defaults:
#
#  CXX                - Your C++ compiler.
#  CPLUS_INCLUDE_PATH - Include file search path.
#  CXXFLAGS           - Extra compiler flags.
#  LDFLAGS            - Extra linker flags.
#  
#------------------------------------------------------------------------------

CXXFLAGS += -O3
CXXFLAGS += -g
CXXFLAGS += -std=c++11 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CXXFLAGS += -I../../libosmium/include

ifneq ($(HANDLER),)
    CXXFLAGS += -DHANDLER=$(HANDLER)
endif

OS:=$(shell uname -s)
ifeq ($(OS),Darwin)
	CXXFLAGS += -stdlib=libc++
	LDFLAGS += -stdlib=libc++
endif

CXXFLAGS_OGR      := $(shell gdal-config --cflags)
CXXFLAGS_WARNINGS := -Wall -Wextra -pedantic -Wredundant-decls -Wdisabled-optimization -Wctor-dtor-privacy -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo -Wold-style-cast

LIB_EXPAT  := -lexpat
LIB_PBF    := -pthread -lz -lprotobuf-lite -losmpbf
LIB_GZIP   := -lz
LIB_BZIP2  := -lbz2

LIB_IO     := $(LIB_EXPAT) $(LIB_PBF) $(LIB_GZIP) $(LIB_BZIP2)

LIB_OGR    := $(shell gdal-config --libs)
LIB_PROJ   := -lproj
LIB_PRGOPT := -lboost_program_options
LIB_FS     := -lboost_system -lboost_filesystem

PROGRAMS := mapolution

.PHONY: all clean

all: $(PROGRAMS)

main.o: main.cpp cmdline_options.hpp gdalcpp.hpp geom_handler.hpp handlers/*.hpp
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_OGR) $(CXXFLAGS_WARNINGS) -o $@ $<

cmdline_options.o: cmdline_options.cpp cmdline_options.hpp
	$(CXX) -c $(CXXFLAGS) $(CXXFLAGS_WARNINGS) -o $@ $<

mapolution: main.o cmdline_options.o
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIB_IO) $(LIB_OGR) $(LIB_PROJ) $(LIB_PRGOPT) $(LIB_FS)

clean:
	rm -f *.o core $(PROGRAMS)

