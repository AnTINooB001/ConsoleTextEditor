CXX=g++
CXX_FLAGS=-g -Wall

TARGET=TEditor

SRC_DIR=.
BIN_DIR=bin

SOURCES=$(SRC_DIR)/*.cpp
DEL=rm -f
LIBS_DIRS=-I./include/ 
STATIC_LIB_DIRS= -I/usr/lib/x86_64-linux-gnu/
LIBS=$(LIBS_DIRS) -lncurses
STATIC_LIB = $(STATIC_LIB_DIRS) -l:libncurses.a -ltinfo

.PHONY: clean build
all: clean build

build: $(SOURCES)
		$(CXX) $(CXX_FLAGS) $(SOURCES) -o $(BIN_DIR)/$(TARGET) $(LIBS)

sbuild: $(SOURCES)
		$(CXX) -static $(CXX_FLAGS) $(SOURCES) -o $(BIN_DIR)/$(TARGET) $(STATIC_LIB)

run:
	./$(BIN_DIR)/$(TARGET)

clean:
	$(DEL_FILE) $(BIN_DIR)/*

