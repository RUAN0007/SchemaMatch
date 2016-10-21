WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

HEADER_DIR = include
INC = -Iinclude
CFLAGS = -Wall -std=c++11
RESINC = 
LIBDIR = -L lib
LIB = -l dice-knowledgebase
LDFLAGS =  -lcurl -ltinyxml -lclucene-shared -lclucene-core

SRC = src
INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/debug
DEP_DEBUG = 
OUTDIR_DEBUG = bin/debug

OBJ_DEBUG =  $(OBJDIR_DEBUG)/WebTable.o $(OBJDIR_DEBUG)/jsoncpp.o $(OBJDIR_DEBUG)/TablePattern.o  \
$(OBJDIR_DEBUG)/SchemaMatching.o  $(OBJDIR_DEBUG)/debug.o $(OBJDIR_DEBUG)/Crowdsourcing.o

OBJ_MAIN_DEBUG = $(OBJDIR_DEBUG)/main.o


MKDIR_P = mkdir -p

INC_TEST = $(INC)
CFLAGS_TEST = $(CFLAGS) -g
RESINC_TEST = $(RESINC)
RCFLAGS_TEST = $(RCFLAGS)
LIBDIR_TEST = $(LIBDIR)
LIB_TEST = $(LIB) -l gtest
LDFLAGS_TEST = $(LDFLAGS) -pthread
OBJDIR_TEST = obj/test
SRC_TEST = test
DEP_TEST = 
OUTDIR_TEST = bin/test
SRC_TEST = test
OBJ_TEST =  $(OBJDIR_TEST)/TablePattern-test.o
OBJ_MAIN_TEST = $(OBJDIR_TEST)/test.o

DIR = $(OBJDIR_DEBUG) $(OBJDIR_TEST) $(OUTDIR_DEBUG) $(OUTDIR_TEST)

all: folder debug test

folder: $(DIR)

$(DIR):
	$(MKDIR_P) $(DIR)

test: $(OBJ_TEST) $(OBJ_MAIN_TEST)
	$(CXX) $(LIBDIR_TEST) $(OBJ_MAIN_TEST) $(OBJ_TEST) $(OBJ_DEBUG) $(LIB_TEST) $(LDFLAGS_TEST) -o $(OUTDIR_TEST)/test

$(OBJDIR_TEST)/test.o: $(SRC_TEST)/test.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC_TEST)/test.cpp -o $(OBJDIR_TEST)/test.o
	
$(OBJDIR_TEST)/TablePattern-test.o: $(SRC_TEST)/TablePattern-test.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC_TEST)/TablePattern-test.cpp -o $(OBJDIR_TEST)/TablePattern-test.o

debug: $(OBJ_DEBUG) $(OBJ_MAIN_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUTDIR_DEBUG)/main $(OBJ_MAIN_DEBUG) $(OBJ_DEBUG)  $(LIB_DEBUG) $(LDFLAGS_DEBUG) 

$(OBJDIR_DEBUG)/WebTable.o: $(SRC)/WebTable.cpp $(HEADER_DIR)/WebTable.h
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC)/WebTable.cpp -o $(OBJDIR_DEBUG)/WebTable.o

$(OBJDIR_DEBUG)/jsoncpp.o: $(SRC)/jsoncpp.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC)/jsoncpp.cpp -o $(OBJDIR_DEBUG)/jsoncpp.o

$(OBJDIR_DEBUG)/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/TablePattern.o: $(SRC)/TablePattern.cpp $(HEADER_DIR)/TablePattern.h
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC)/TablePattern.cpp -o $(OBJDIR_DEBUG)/TablePattern.o

$(OBJDIR_DEBUG)/SchemaMatching.o: $(SRC)/SchemaMatching.cpp $(HEADER_DIR)/SchemaMatching.h
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC)/SchemaMatching.cpp -o $(OBJDIR_DEBUG)/SchemaMatching.o

$(OBJDIR_DEBUG)/debug.o: $(SRC)/debug.cpp $(HEADER_DIR)/debug.h
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC)/debug.cpp -o $(OBJDIR_DEBUG)/debug.o
	
$(OBJDIR_DEBUG)/Crowdsourcing.o: $(SRC)/Crowdsourcing.cpp $(HEADER_DIR)/Crowdsourcing.h
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c $(SRC)/Crowdsourcing.cpp -o $(OBJDIR_DEBUG)/Crowdsourcing.o
clean: 
	rm -rf $(OBJDIR_DEBUG) $(OBJDIR_TEST)
	rm -rf $(OUTDIR_DEBUG) $(OUTDIR_TEST) 
	