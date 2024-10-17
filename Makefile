# compiler rules
CC = gcc
CFLAGS = -Wall -Wno-builtin-declaration-mismatch -Iinclude -Iinclude/extern_include
LDFLAGS = -Llib -ltimber

# directories
BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

# yeah dont ask me
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

EXECUTABLE = $(BIN_DIR)/mako

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE)

install: $(EXECUTABLE)
	cp $(EXECUTABLE) /usr/local/bin

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean



































