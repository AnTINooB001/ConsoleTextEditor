CXX=g++
CXX_FLAGS=-g -Wall

TARGET=example

SRC_DIR=.
BIN_DIR=bin

SOURCES=$(SRC_DIR)/*.cpp
DEL=rm -f
LIBS_DIRS=-I./include/
LIBS=$(LIBS_DIRS) -lncurses
STATIC_LIB=$(LIBS_DIRS) -l:libncurses.a	

.PHONY: clean build
all: clean build

build: $(SOURCES)
		$(CXX) $(CXX_FLAGS) $(SOURCES) -o $(BIN_DIR)/$(TARGET) $(LIBS)

run:
	./$(BIN_DIR)/$(TARGET)

clean:
	$(DEL_FILE) $(BIN_DIR)/*

