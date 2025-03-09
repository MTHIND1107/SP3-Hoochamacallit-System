# Makefile for Data Reader (DR)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -g
INC_DIR = inc

# Make sure directories exist
$(shell mkdir -p bin obj)

# Target to build the final executable
bin/DataReader: obj/DataReader.o
	$(CC) $(CFLAGS) obj/DataReader.o -o bin/DataReader

# Rule for DataReader.o
obj/DataReader.o: src/DataReader.c $(INC_DIR)/drcommon.h
	$(CC) $(CFLAGS) -c src/DataReader.c -I$(INC_DIR) -o obj/DataReader.o

# Default target to build everything
all: bin/DataReader

# Rule to clean up generated files
clean:
	rm -f obj/*.o bin/DataReader

.PHONY: all clean
