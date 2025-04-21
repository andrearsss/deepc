# Compiler and flags
CC = gcc
CFLAGS = -Wall -I./include

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Files
TARGET = deepc
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/matrix.c
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=%.o)

# Default target: build the project
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET)

# Compile .c files to .o files
%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the generated files
clean:
	rm -rf $(OBJ_FILES) $(TARGET)

# Phony targets
.PHONY: all clean
