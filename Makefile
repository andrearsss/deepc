# Compiler and flags
CC = gcc
F_CC = -Wall -I./include
F_ASAN = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
F_BLAS = -lopenblas 

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test

# Files for main target
TARGET = deepc
MAIN_SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/matrix.c
MAIN_OBJ_FILES = $(MAIN_SRC_FILES:$(SRC_DIR)/%.c=%.o)

# Files for test target
TEST_TARGET = $(TEST_DIR)/test_main
TEST_MAIN_SRC = $(TEST_DIR)/test_main.c
TEST_MAIN_OBJ = $(TEST_MAIN_SRC:.c=.o)
LIB_SRC = $(SRC_DIR)/matrix.c
LIB_OBJ = matrix.o

all: $(TARGET)

asan: $(MAIN_OBJ_FILES)
	$(CC) $(MAIN_OBJ_FILES) -o $(TARGET) $(F_ASAN)

blas: $(MAIN_OBJ_FILES)
	$(CC) $(MAIN_OBJ_FILES) -o $(TARGET) $(F_BLAS) $(F_ASAN)

$(TARGET): $(MAIN_OBJ_FILES)
	$(CC) $(MAIN_OBJ_FILES) -o $(TARGET)

# --- Test Target ---
test: $(TEST_TARGET)
	$(TEST_TARGET) 

$(TEST_TARGET): $(TEST_MAIN_OBJ) $(LIB_OBJ)
	$(CC) $^ -o $@ -lopenblas -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer

$(TEST_MAIN_OBJ): $(TEST_MAIN_SRC) $(INCLUDE_DIR)/matrix.h
	$(CC) $(F_CC) -c $< -o $@

$(LIB_OBJ): $(SRC_DIR)/matrix.c $(INCLUDE_DIR)/matrix.h
	$(CC) $(F_CC) -c $< -o $@

# --- Generic rule for object files from src ---
%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/matrix.h
	$(CC) $(F_CC) -c $< -o $@

clean:
	rm -rf $(MAIN_OBJ_FILES) $(TARGET) $(TEST_TARGET) $(TEST_MAIN_OBJ) $(LIB_OBJ)

.PHONY: all clean asan blas test
