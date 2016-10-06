WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -Iinclude
CFLAGS = -Wall -std=c++11
RESINC = 
LIBDIR = -L lib
LIB = -l dice-knowledgebase
LDFLAGS =  -lcurl -ltinyxml -lclucene-shared -lclucene-core

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/SchemaMatching

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/main.o $(OBJDIR_DEBUG)/src/WebTable.o $(OBJDIR_DEBUG)/src/jsoncpp.o $(OBJDIR_DEBUG)/src/TablePattern.o

all: debug
	$(OUT_DEBUG)
clean: clean_debug

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LIB_DEBUG) $(LDFLAGS_DEBUG) 

$(OBJDIR_DEBUG)/src/WebTable.o: src/WebTable.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/WebTable.cpp -o $(OBJDIR_DEBUG)/src/WebTable.o

$(OBJDIR_DEBUG)/src/jsoncpp.o: src/jsoncpp.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/jsoncpp.cpp -o $(OBJDIR_DEBUG)/src/jsoncpp.o

$(OBJDIR_DEBUG)/src/main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)/src/main.o

$(OBJDIR_DEBUG)/src/TablePattern.o: src/TablePattern.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/TablePattern.cpp -o $(OBJDIR_DEBUG)/src/TablePattern.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)/src

.PHONY: before_debug after_debug clean_debug 
