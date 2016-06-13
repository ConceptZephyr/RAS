R$(MC_LIB)EMOVE=/bin/rm -f
MOVE=/bin/mv
COPY=/bin/cp
LINK=ln -s
MAKE=/usr/bin/make
BIN_DIR=../../bin
LIB_DIR=../../lib
THIRD_PARTY_DIR=../../thirdparty
UTIL_DIR=../../util/src/
UTIL_LIB=$(LIB_DIR)/libutil.a
OS=OSX

CC=g++
CFLAGS=-c -Wall -Os
INCFLAGS=-I . -I $(UTIL_DIR)
LDFLAGS=-L $(LIB_DIR) -l util
ARCHIVE=/usr/bin/ar r

print-%  : ; @echo $* = $($*)

%.o: %.c %.h Makefile


.cpp.o: $(COMMON_HEADERS) Makefile
	$(CC) $(ZMQ_INCFLAGS) $(CFLAGS) $< -o $@

################################################################################
#
# ZMQ
#
################################################################################

ZMQ_COMMON_HEADERS=connect_util.h zmq_version.h $(UTIL_DIR)/am_constants.h $(UTIL_DIR)/am_split_string.h $(UTIL_DIR)/am_util.h

ZMQ_SOURCES=connect_util.cpp zmq_read.cpp zmq_write.cpp zmq_hub_class.cpp
ZMQ_OBJECTS=$(ZMQ_SOURCES:.cpp=.o)
ZMQ_HEADERS=$(ZMQ_SOURCES:.cpp=.h)

ZMQ_LIB_ROOT=amzmq
ZMQ_LIB=lib$(ZMQ_LIB_ROOT).a

ZMQ_INCFLAGS=$(INCFLAGS) -I $(THIRD_PARTY_DIR)/include/zmq
ZMQ_LDFLAGS=$(LDFLAGS) -L $(THIRD_PARTY_DIR)/$(OS)/lib -l zmq
ZMQ_EXE_LDFLAGS=$(ZMQ_LDFLAGS) -l  $(ZMQ_LIB_ROOT)

ZMQ_EXECUTABLE_01=stdin2zmq
ZMQ_MAIN_01=stdin_2_zmq.cpp
ZMQ_SOURCES_01=$(ZMQ_MAIN_01)
ZMQ_OBJECTS_01=$(ZMQ_MAIN_01:.cpp=.o)

ZMQ_EXECUTABLE_02=zmq2stdout
ZMQ_MAIN_02=zmq_2_stdout.cpp
ZMQ_SOURCES_02=$(ZMQ_MAIN_02)
ZMQ_OBJECTS_02=$(ZMQ_MAIN_02:.cpp=.o)

ZMQ_EXECUTABLE_03=file2zmq
ZMQ_MAIN_03=file_2_zmq.cpp
ZMQ_SOURCES_03=$(ZMQ_MAIN_03)
ZMQ_OBJECTS_03=$(ZMQ_MAIN_03:.cpp=.o)

ZMQ_EXECUTABLE_04=zmq2file
ZMQ_MAIN_04=zmq_2_file.cpp
ZMQ_SOURCES_04=$(ZMQ_MAIN_04)
ZMQ_OBJECTS_04=$(ZMQ_MAIN_04:.cpp=.o)

ZMQ_EXECUTABLE_05=zmqhub
ZMQ_MAIN_05=zmq_hub.cpp
ZMQ_SOURCES_05=$(ZMQ_MAIN_05)
ZMQ_OBJECTS_05=$(ZMQ_MAIN_05:.cpp=.o)


ZMQ_EXECUTABLES=$(ZMQ_EXECUTABLE_01) $(ZMQ_EXECUTABLE_02) $(ZMQ_EXECUTABLE_03) $(ZMQ_EXECUTABLE_04) $(ZMQ_EXECUTABLE_05)
ZMQ_ALL_OBJECTS=$(ZMQ_OBJECTS_01) $(ZMQ_OBJECTS_02) $(ZMQ_OBJECTS_03) $(ZMQ_OBJECTS_04) $(ZMQ_OBJECTS_05)

all: $(MC_EXECUTABLE_01) $(ZMQ_LIB) $(ZMQ_LIB) $(ZMQ_EXECUTABLE_01) $(ZMQ_EXECUTABLE_02) $(ZMQ_EXECUTABLE_03) $(ZMQ_EXECUTABLE_04) $(ZMQ_EXECUTABLE_05) $(MC_LIB)
	ls $(MC_EXECUTABLE_01)

zmq: $(ZMQ_LIB) $(ZMQ_EXECUTABLE_01) $(ZMQ_EXECUTABLE_02) $(ZMQ_EXECUTABLE_03) $(ZMQ_EXECUTABLE_04) $(ZMQ_EXECUTABLE_05)

$(ZMQ_LIB): $(ZMQ_OBJECTS) Makefile
	$(ARCHIVE) $(ZMQ_LIB) $(ZMQ_OBJECTS)

zmq_clean:
	$(REMOVE) $(ZMQ_OBJECTS)
	$(REMOVE) $(ZMQ_LIB)
	$(REMOVE) $(ZMQ_EXECUTABLES)
	$(REMOVE) $(ZMQ_ALL_OBJECTS)

zmq_clean_all:
	make zmq_clean
	$(REMOVE) $(LIB_DIR)/$(ZMQ_LIB)

zmq_rebuild:
	make zmq_clean_all; make zmq

zmq_install:
	make zmq; \
	$(COPY) $(ZMQ_LIB) $(LIB_DIR); \
	$(COPY) $(ZMQ_EXECUTABLE_01) $(BIN_DIR); \
	$(COPY) $(ZMQ_EXECUTABLE_02) $(BIN_DIR); \
	$(COPY) $(ZMQ_EXECUTABLE_03) $(BIN_DIR); \
	$(COPY) $(ZMQ_EXECUTABLE_04) $(BIN_DIR); \
	$(COPY) $(ZMQ_EXECUTABLE_05) $(BIN_DIR); \


zmq_install_clean:
	make zmq
	make zmq_install
	make zmq_clean


$(ZMQ_EXECUTABLE_01): $(ZMQ_MAIN_01) $(ZMQ_SOURCES) $(ZMQ_ALL_OBJECTS_01) $(ZMQ_HEADERS_01) $(ZMQ_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(ZMQ_INCFLAGS) $(CFLAGS) $(ZMQ_MAIN_01)
	$(CC) $(ZMQ_INCFLAGS) $(ZMQ_ALL_OBJECTS_01) $(ZMQ_EXE_LDFLAGS) $(ZMQ_OBJECTS_01) -o $@
	$(COPY) $(ZMQ_EXECUTABLE_01) $(BIN_DIR)

$(ZMQ_EXECUTABLE_02): $(ZMQ_MAIN_02) $(ZMQ_SOURCES) $(ZMQ_ALL_OBJECTS_02) $(ZMQ_HEADERS_02) $(ZMQ_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(ZMQ_INCFLAGS) $(CFLAGS) $(ZMQ_MAIN_02)
	$(CC) $(ZMQ_INCFLAGS) $(ZMQ_ALL_OBJECTS_02) $(ZMQ_EXE_LDFLAGS) $(ZMQ_OBJECTS_02) -o $@
	$(COPY) $(ZMQ_EXECUTABLE_02) $(BIN_DIR)

$(ZMQ_EXECUTABLE_03): $(ZMQ_MAIN_03) $(ZMQ_SOURCES) $(ZMQ_ALL_OBJECTS_03) $(ZMQ_HEADERS_03) $(ZMQ_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(ZMQ_INCFLAGS) $(CFLAGS) $(ZMQ_MAIN_03)
	$(CC) $(ZMQ_INCFLAGS) $(ZMQ_ALL_OBJECTS_03) $(ZMQ_EXE_LDFLAGS) $(ZMQ_OBJECTS_03) -o $@
	$(COPY) $(ZMQ_EXECUTABLE_03) $(BIN_DIR)

$(ZMQ_EXECUTABLE_04): $(ZMQ_MAIN_04) $(ZMQ_SOURCES) $(ZMQ_ALL_OBJECTS_04) $(ZMQ_HEADERS_04) $(ZMQ_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(ZMQ_INCFLAGS) $(CFLAGS) $(ZMQ_MAIN_04)
	$(CC) $(ZMQ_INCFLAGS) $(ZMQ_ALL_OBJECTS_04) $(ZMQ_EXE_LDFLAGS) $(ZMQ_OBJECTS_04) -o $@
	$(COPY) $(ZMQ_EXECUTABLE_04) $(BIN_DIR)

$(ZMQ_EXECUTABLE_05): $(ZMQ_MAIN_05) $(ZMQ_SOURCES) $(ZMQ_ALL_OBJECTS_05) $(ZMQ_HEADERS_05) $(ZMQ_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(ZMQ_INCFLAGS) $(CFLAGS) $(ZMQ_MAIN_05)
	$(CC) $(ZMQ_INCFLAGS) $(ZMQ_ALL_OBJECTS_05) $(ZMQ_EXE_LDFLAGS) $(ZMQ_OBJECTS_05) -o $@
	$(COPY) $(ZMQ_EXECUTABLE_05) $(BIN_DIR)


################################################################################
#
# Multicast
#
################################################################################


MC_SOURCES_READ=connect_util.cpp am_multicast_read.cpp 
MC_OBJECTS_READ=$(MC_SOURCES_READ:.cpp=.o)
MC_HEADERS_READ=$(MC_SOURCES_READ:.cpp=.h)

MC_SOURCES_WRITE=connect_util.cpp am_multicast_write.cpp
MC_OBJECTS_WRITE=$(MC_SOURCES_WRITE:.cpp=.o)
MC_HEADERS_WRITE=$(MC_SOURCES_WRITE:.cpp=.h)

MC_SOURCES=connect_util.cpp am_multicast_read.cpp am_multicast_write.cpp
MC_OBJECTS=$(MC_SOURCES:.cpp=.o)
MC_HEADERS=$(MC_SOURCES:.cpp=.h)

MCINCFLAGS=$(INCFLAGS)
MCLDFLAGS=$(LDFLAGS)
MCEXE_LDFLAGS=$(LDFLAGS)

MC_LIB_ROOT=connect
MC_LIB=lib$(MC_LIB_ROOT).a

MC_EXECUTABLE_01=stdin2mc
MC_MAIN_01=stdin_2_mc.cpp
MC_SOURCES_01=$(MC_MAIN_01) $(MC_SOURCES_WRITE)
MC_OBJECTS_01=$(MC_SOURCES_01:.cpp=.o)

#MC_EXECUTABLE_02=mc2stdout
#MC_MAIN_02=mc_2_stdout.cpp
#MC_SOURCES_02=$(MC_MAIN_02)
#MC_OBJECTS_02=$(MC_MAIN_02:.cpp=.o)

#MC_EXECUTABLE_03=file2mc
#MC_MAIN_03=file_2_mc.cpp
#MC_SOURCES_03=$(MC_MAIN_03)
#MC_OBJECTS_03=$(MC_MAIN_03:.cpp=.o)

#MC_EXECUTABLE_04=mc2file
#MC_MAIN_04=mc_2_file.cpp
#MC_SOURCES_04=$(MC_MAIN_04)
#MC_OBJECTS_04=$(MC_MAIN_04:.cpp=.o)

MC_INCFLAGS=$(INCFLAGS)
MC_EXE_LDFLAGS=$(LDFLAGS)

$(MC_LIB): $(MC_SOURCES) $(MC_HEADERS)
	$(CC) $(MC_INCFLAGS) $(CFLAGS) $(MC_SOURCES)
	$(ARCHIVE) $(MC_LIB) $(MC_OBJECTS)

$(MC_EXECUTABLE_01): $(MC_MAIN_01) $(MC_SOURCES_WRITE) $(MC_OBJECTS_01) $(MC_HEADERS_01) $(MC_COMMON_HEADERS) $(UTIL_LIB) Makefile
	$(CC) $(MC_INCFLAGS) $(CFLAGS) $(MC_MAIN_01)
	$(CC) $(MC_INCFLAGS) $(MC_EXE_LDFLAGS) $(MC_OBJECTS_01) -o $@
	$(COPY) $(MC_EXECUTABLE_01) $(BIN_DIR)

#llllllll
#$(MC_EXECUTABLE_02): $(MC_MAIN_02) $(MC_SOURCES) $(MC_ALL_OBJECTS_02) $(MC_HEADERS_02) $(MC_COMMON_HEADERS) $(UTIL_LIB) Makefile
#	$(CC) $(MC_INCFLAGS) $(CFLAGS) $(MC_MAIN_02)
#	$(CC) $(MC_INCFLAGS) $(MC_ALL_OBJECTS_02) $(MC_EXE_LDFLAGS) $(MC_OBJECTS_02) -o $@
#	$(COPY) $(MC_EXECUTABLE_02) $(BIN_DIR)

#$(MC_EXECUTABLE_03): $(MC_MAIN_03) $(MC_SOURCES) $(MC_ALL_OBJECTS_03) $(MC_HEADERS_03) $(MC_COMMON_HEADERS) $(UTIL_LIB) Makefile
#	$(CC) $(MC_INCFLAGS) $(CFLAGS) $(MC_MAIN_03)
#	$(CC) $(MC_INCFLAGS) $(MC_ALL_OBJECTS_03) $(MC_EXE_LDFLAGS) $(MC_OBJECTS_03) -o $@
#	$(COPY) $(MC_EXECUTABLE_03) $(BIN_DIR)

#$(MC_EXECUTABLE_04): $(MC_MAIN_04) $(MC_SOURCES) $(MC_ALL_OBJECTS_04) $(MC_HEADERS_04) $(MC_COMMON_HEADERS) $(UTIL_LIB) Makefile
#	$(CC) $(MC_INCFLAGS) $(CFLAGS) $(MC_MAIN_04)
#	$(CC) $(MC_INCFLAGS) $(MC_ALL_OBJECTS_04) $(MC_EXE_LDFLAGS) $(MC_OBJECTS_04) -o $@
#	$(COPY) $(MC_EXECUTABLE_04) $(BIN_DIR)

mc_clean:
	$(REMOVE) $(MC_OBJECTS)
	$(REMOVE) $(MC_LIB)

mc_clean_all:
	make mc_clean
	$(REMOVE) $(LIB_DIR)/$(MC_LIB)

mc_rebuild:
	make mc_clean_all; make $(MC_LIB)

mc_install:
	make $(MC_LIB)
	$(COPY) $(MC_LIB) $(LIB_DIR)

mc_install_clean:
	make $(MC_LIB)
	make mc_install
	make mc_clean



################################################################################
#
# ALL
#
################################################################################

SOURCES=$(MC_SOURCES) $(ZMQ_SOURCES)
OBJECTS=$(MC_OBJECTS) $(ZMQ_OBJECTS)
HEADERS=$(MC_HEADERS) $(ZMQ_HEADERS)


clean:
	make mc_clean
	make zmq_clean

clean_all:
	make mc_clean_all
	make zmq_clean_all

rebuild:
	make mc_rebuild
	make zmq_rebuild

install:
	make mc_install
	make zmq_install

install_clean:
	make mc_install_clean
	make zmq_install_clean

