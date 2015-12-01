############################################################
# Makefile for Benchmarking Skyline Algorithms             #
#   Copyright (c) 2015                                     #
#   Darius Sidlauskas (darius.sidlauskas@epfl.ch)          #
#   Sean Chester (sean.chester@idi.ntnu.no)                #
############################################################

RM = rm -rf
MV = mv
CP = cp -rf
CC = g++-5

TARGET = $(OUT)/ParallelTKDQ

SRC = $(wildcard src/util/*.cpp) \
	  $(wildcard src/common/*.cpp) \
  	  $(wildcard src/naive/*.cpp) \
  	  $(wildcard src/refinement/*.cpp) \
  	  $(wildcard src/partition_based/*.cpp) \
      $(wildcard src/*.cpp)

OBJ = $(addprefix $(OUT)/,$(notdir $(SRC:.cpp=.o)))

OUT = bin

LIB_DIR = # used as -L$(LIB_DIR)
INCLUDES = -I ./src/

LIB = 

# Forces make to look these directories
VPATH = src:src/util:src/naive:src/refinement:src/partition_based:src/common

# By default compiling for performance (optimal)
CXXFLAGS = -O3 -m64 -DNDEBUG\
       -Wno-deprecated -Wno-write-strings -nostdlib -Wpointer-arith \
       -Wcast-qual -Wcast-align \
       -std=c++0x -fopenmp -mavx -march=native 
           
LDFLAGS=-m64 -fopenmp #-lrt -lpapi

# Target-specific Variable values:
# Compile for debugging (works with valgrind)
dbg : CXXFLAGS = -O0 -g3 -m64\
		-Wno-deprecated -Wno-write-strings -nostdlib -Wpointer-arith \
    -Wcast-qual -Wcast-align -std=c++0x
dbg : all

# All Target
all: $(TARGET)

# Tool invocations
$(TARGET): $(OBJ) $(LIB_DIR)$(LIB)
	@echo 'Building target: $@ (GCC C++ Linker)'
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)
	@echo 'Finished building target: $@'
	@echo ' '

$(OUT)/%.o: %.cpp
	@echo 'Building file: $< (GCC C++ Compiler)'
	$(CC) $(CXXFLAGS) $(INCLUDES) -c -o"$@" "$<" 
	@echo 'Finished building: $<'
	@echo ' '

clean:
	-$(RM) $(OBJ) $(TARGET) $(addprefix $(OUT)/,$(notdir $(SRC:.cpp=.d)))
	-@echo ' '

deepclean:
	-$(RM) bin/*
	-@echo ' '


.PHONY: all clean deepclean dbg tests
