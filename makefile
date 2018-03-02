GCC = gcc
CPP = g++
LFLAGS = -lpthread 
SRC = src
OBJ = obj
BIN = bin
WWW = www
CSRC = cgi-src
CBIN = www/cgi-bin
INCD = include 
INC = -Iinclude
DIRS := $(WWW) $(SRC) $(OBJ) $(BIN) $(CSRC) $(CBIN) $(INCD)
HEADERS := $(wildcard inlcude/*.hpp)
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:$(SRC)/%.cpp=$(OBJ)/%.o)
CGIS := $(wildcard $(CSRC)/*.cpp)
CGIB := $(CGIS:$(CSRC)/%.cpp=$(CBIN)/%.cgi)
EXEC = $(BIN)/web_server

CFLAGS = -O3 -g -Wall -Wextra -std=c++11 $(INC)

.PHONY: all clean
all: $(EXEC) $(CGIB) | $(DIRS)

$(EXEC): $(OBJECTS) | $(DIRS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o $(EXEC) $(LFLAGS)

$(OBJECTS): $(OBJ)/%.o : $(SRC)/%.cpp | $(DIRS)
	$(CPP) $(CFLAGS) -c $< -o $@

$(CGIB): $(CBIN)/% : $(CGIS) | $(DIRS)
	$(CPP) $(CFLAGS) -o $@ $<

$(DIRS):
	mkdir $@

clean:
	@rm -f $(OBJECTS)
	@rm -f $(EXEC)
	@rm -f $(CGIB)
