# Compilation
CC := gcc
CFLAGS := -Wall -Werror
TESTFLAGS := -g -DDEBUG

# Directories
BIN := bin
PROG := progs
HDR := includes
SRC := src
OBJ := obj
TEST := tests

# Derived directories
PROG_BIN := $(BIN)/$(PROG)
TEST_BIN := $(BIN)/$(TEST)

SRCS := $(wildcard $(SRC)/*.c)
HDRS := $(wildcard $(HDR)/*.h)
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
PROG_SRCS := $(wildcard $(PROG)/*.c)
PROGS := $(patsubst $(PROG)/%.c, $(PROG_BIN)/%, $(PROG_SRCS))
TEST_SRCS := $(wildcard $(TEST)/*.c)
TESTS := $(patsubst $(TEST)/%.c, $(TEST_BIN)/%, $(TEST_SRCS))

BUILD_DIR := $(shell mkdir -p $(OBJ) $(PROG_BIN) $(TEST_BIN))

all: $(PROGS)

# Building programs
$(PROG_BIN)/%: $(PROG)/%.c $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -I $(HDR)

# Compiling to .o
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I $(HDR)

# Unit testing
$(TEST_BIN)/%_test: CFLAGS := $(CFLAGS) $(TESTFLAGS)
$(TEST_BIN)/%_test: $(TEST)/%_test.c $(filter-out $(OBJ)/main.o, $(OBJS))
	$(CC) $(CFLAGS) $(TESTFLAGS) $^ -o $@ -I $(HDR)

test: $(TESTS)
	@echo $(TESTS) | xargs -n 1 sh -c

atest: $(TEST_BIN)/$(T)_test
	$(TEST_BIN)/$(T)_test

echo: $(filter-out $(OBJ)/main.o, $(OBJS))
	@echo $^

# Cleaning.
clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)

