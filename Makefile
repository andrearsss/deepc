# Compiler and flags
CC = gcc
F_CC = -Wall -I./include
F_ASAN = -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
F_BLAS = -lopenblas

# Source files
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test

SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/matrix.c $(SRC_DIR)/dense.c $(SRC_DIR)/error.c $(SRC_DIR)/activations.c
OBJ_FILES = $(SRC_FILES:.c=.o)

TEST_SRC_FILES = $(TEST_DIR)/test_main.c $(SRC_DIR)/matrix.c $(SRC_DIR)/dense.c $(SRC_DIR)/error.c $(SRC_DIR)/activations.c
TEST_OBJ_FILES = $(TEST_SRC_FILES:.c=.o)

# Output binaries
BIN = deepc
BIN_ASAN = deepc_asan
BIN_BLAS = deepc_blas
BIN_TEST = $(TEST_DIR)/test_main

.PHONY: all asan blas test clean

all: $(BIN)

asan: CFLAGS += $(F_ASAN)
asan: $(BIN_ASAN)

blas: CFLAGS += $(F_ASAN) $(F_BLAS)
blas: $(BIN_BLAS)

test: CFLAGS += -I$(INCLUDE_DIR) $(F_CC) $(F_ASAN) $(F_BLAS)
test: $(BIN_TEST)
	$(BIN_TEST)

# Build rules
$(BIN): $(OBJ_FILES)
	$(CC) $(F_CC) $^ -o $@

$(BIN_ASAN): $(OBJ_FILES)
	$(CC) $(F_CC) $(F_ASAN) $^ -o $@

$(BIN_BLAS): $(OBJ_FILES)
	$(CC) $(F_CC) $(F_ASAN) $(F_BLAS) $^ -o $@

$(BIN_TEST): $(TEST_OBJ_FILES)
	$(CC) $(F_CC) $(F_ASAN) $(F_BLAS) $^ -o $@

%.o: %.c
	$(CC) $(F_CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ_FILES) $(BIN) $(BIN_ASAN) $(BIN_BLAS) $(BIN_TEST)
