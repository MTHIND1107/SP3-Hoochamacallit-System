# Makefile for Data Corruptor (DX)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -g
INCLUDES = -I./inc

# Directories
BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src

# Make sure the directories exist
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR))

# Source files and objects
SRC = $(wildcard $(SRC_DIR)/*.c) $(SRC_DIR)/DataCorruptor.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Main target
all: $(BIN_DIR)/DataCorruptor

$(BIN_DIR)/DataCorruptor: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lrt

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up
clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*.o

.PHONY: all clean