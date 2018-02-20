GCC = gcc
CPP = g++
LFLAGS = 
SRC = src
OBJ = obj
INC = -Iinclude
HEADERS := $(wildcard inlcude/*.hpp)
SOURCES := $(wildcard src/*.cpp)
#OBJECTS = $(patsubst $(SRC)/%,$(OBJ)/%.o,$(basename $(SOURCES)))
OBJECTS  := $(SOURCES:$(SRC)/%.cpp=$(OBJ)/%.o)
EXEC = bin/web_server

CFLAGS =  -g -Wall -Wextra -std=c++11 $(INC)

.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -o $(EXEC) $(LFLAGS)

#obj/main.o:
#	$(CPP) $(CFLAGS) -c src/main.cpp -o obj/main.o

$(OBJECTS): $(OBJ)/%.o : $(SRC)/%.cpp
	$(CPP) $(CFLAGS) -c $< -o $@
	echo "Compiled "$<" successfully!"


#.cpp.o:
#	$(CPP) $(CFLAGS) -c $< -o $(subst $(SRC),$(OBJ),$<)

clean:
	@rm $(OBJECTS)
	@rm $(EXEC)
