BIN_DIR=../../bin
LIB_DIR=../../lib

CC=g++
CFLAGS_STATIC=-c -Wall -Os
CFLAGS_SHARED=-c -Wall -Os -fPIC
INCFLAGS=-I .
ARCHIVE=/usr/bin/ar r

REMOVE=/bin/rm -f
MOVE=/bin/mv
COPY=/bin/cp
LINK=ln -s
MAKE=/usr/bin/make

SOURCES=am_util.cpp am_string.cpp am_split_string.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=$(SOURCES:.cpp=.h)

STATIC_LIB=libutil.a
SHARED_LIB=libutil.dylib
CURRENT_VERSION=1.0

%.o: %.c %.h Makefile

.cpp.o: $(COMMON_HEADERS) Makefile
	$(CC) $(INCFLAGS) $(CFLAGS) $< -o $@

all: $(STATIC_LIB)

$(STATIC_LIB): $(SOURCES) $(HEADERS)
	$(CC) $(INCFLAGS) $(CFLAGS_STATIC) $(SOURCES)
	$(ARCHIVE) $(STATIC_LIB) $(OBJECTS)
	$(COPY) $(STATIC_LIB) $(LIB_DIR)

shared:
	$(REMOVE) $(OBJECTS)
	$(CC) $(INCFLAGS) $(CFLAGS_SHARED) $(SOURCES)
	$(CC) -dynamiclib -o $(SHARED_LIB).$(CURRENT_VERSION) $(INCFLAGS) $(OBJECTS)

clean:
	$(REMOVE) $(OBJECTS)
	$(REMOVE) $(STATIC_LIB)

clean_all:
	make clean
	$(REMOVE) $(STATIC_LIB)
	$(REMOVE) $(SHARED_LIB).$(CURRENT_VERSION)

rebuild:
	make clean_all; make all

install:
	make all
	$(COPY) $(STATIC_LIB) $(LIB_DIR)
#	$(COPY) $(SHARED_LIB).$(CURRENT_VERSION) $(LIB_DIR); cd $(LIB_DIR); $(REMOVE) $(SHARED_LIB); $(LINK) $(SHARED_LIB).$(CURRENT_VERSION) $(SHARED_LIB)

install_clean:
	make all
	make install
	make clean

