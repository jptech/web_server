GCC = gcc
CPP = g++
LFLAGS = -lpthread 
SRC = src
OBJ = obj
BIN = bin
INC = -Iinclude
HEADERS := $(wildcard inlcude/*.hpp)
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:$(SRC)/%.cpp=$(OBJ)/%.o)
EXEC = $(BIN)/web_server

CFLAGS = -O3 -g -Wall -Wextra -std=c++11 $(INC)

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o $(EXEC) $(LFLAGS)

$(OBJECTS): $(OBJ)/%.o : $(SRC)/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJECTS)
	@rm -f $(EXEC)
