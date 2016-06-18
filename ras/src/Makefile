OS=OSX
CC=g++
ARCHIVE=/usr/bin/ar r

THIRD_PARTY_DIR=../../thirdparty
ZMQ_INCFLAGS=-I $(THIRD_PARTY_DIR)/include/zmq
ZMQ_LDFLAGS=-L $(THIRD_PARTY_DIR)/$(OS)/lib -l zmq

INCFLAGS=-I. -I../../util/src -I../../ant/src -I../../connect/src $(ZMQ_INCFLAGS)

REMOVE=/bin/rm -f
MOVE=/bin/mv
COPY=/bin/cp
LINK=ln -s
MAKE=/usr/bin/make
BIN_DIR=../../bin
LIB_DIR=../../lib
UTIL_LIB=$(LIB_DIR)/libutil.a
ANT_LIB=$(LIB_DIR)/libant.a
ZMQ_LIB=$(LIB_DIR)/libzmq.a
AM_ZMQ_LIB=$(LIB_DIR)/libamzmq.a
LDFLAGS=-L $(LIB_DIR) -l util
ANT_LDFLAGS=-l ant
ZMQ_LDFLAGS+=-l amzmq


SOURCES=quantity.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HEADERS=$(SOURCES:.cpp=.h)

%.o: %.c %.h Makefile

.cpp.o: $(COMMON_HEADERS) Makefile
	$(CC) $(INCFLAGS) $(CFLAGS) $< -c -o $@

QTY_EXECUTABLE=test_quantity
QTY_MAIN=$(QTY_EXECUTABLE).cpp
QTY_SOURCES=$(QTY_MAIN)
QTY_HEADERS=$(HEADERS)
QTY_OBJECTS=$(QTY_MAIN:.cpp=.o)
QTY_ALL_OBJECTS=$(QTY_OBJECTS) $(OBJECTS)
QTY_INCFLAGS=$(INCFLAGS)
QTY_EXE_LDFLAGS=$(LDFLAGS)

RAS_EXECUTABLE=ras
RAS_MAIN=$(RAS_EXECUTABLE).cpp
RAS_SOURCES=$(RAS_MAIN) computation_engine.cpp 
RAS_HEADERS=$(HEADERS) computation_engine.h ras_constants.h 
RAS_OBJECTS=$(RAS_SOURCES:.cpp=.o)
RAS_ALL_OBJECTS=$(RAS_OBJECTS) $(OBJECTS)
RAS_INCFLAGS=$(INCFLAGS)
RAS_EXE_LDFLAGS=$(LDFLAGS) $(ANT_LDFLAGS) $(ZMQ_LDFLAGS)


all: $(QTY_EXECUTABLE) $(QTY_SOURCES) $(QTY_HEADERS) $(RAS_EXECUTABLE) $(RAS_SOURCES) $(RAS_HEADERS) $(UTIL_LIB) $(ANT_LIB) $(ZMQ_LIB) $(AM_ZMQ_LIB) Makefile


$(QTY_EXECUTABLE): $(QTY_MAIN) $(QTY_SOURCES) $(QTY_ALL_OBJECTS) $(QTY_HEADERS) $(QTY_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(QTY_INCFLAGS) $(CFLAGS) -c $(QTY_MAIN)
	$(CC) $(QTY_INCFLAGS) $(QTY_ALL_OBJECTS) $(QTY_EXE_LDFLAGS) -o $@

$(RAS_EXECUTABLE): $(RAS_MAIN) $(RAS_SOURCES) $(RAS_ALL_OBJECTS) $(RAS_HEADERS) $(RAS_COMMON_HEADERS) $(UTIL_LIB) $(ANT_LIB) $(ZMQ_LIB) $(AM_ZMQ_LIB) Makefile
	$(CC) $(RAS_INCFLAGS) $(CFLAGS) -c $(RAS_MAIN)
	$(CC) $(RAS_INCFLAGS) $(RAS_ALL_OBJECTS) $(RAS_EXE_LDFLAGS) -o $@

clean:
	$(REMOVE) $(OBJECTS) $(QTY_OBJECTS) $(RAS_OBJECTS)

rebuild:
	make clean_all; make all

install:
	make all
	$(COPY) $(QTY_EXECUTABLE) $(BIN_DIR)
	$(COPY) $(RAS_EXECUTABLE) $(BIN_DIR)

install_clean:
	make all
	make install
	make clean

