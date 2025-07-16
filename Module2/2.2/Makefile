CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/task2.2
TEST_TARGET = $(BUILD_DIR)/test_operations

all: $(TARGET) test

$(TARGET): $(BUILD_DIR)/operations.o $(BUILD_DIR)/task2.2.o
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(TEST_TARGET): $(BUILD_DIR)/operations.o $(BUILD_DIR)/test_operations.o
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(shell pkg-config --libs check)

test: $(TEST_TARGET)
	@$(TEST_TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/operations.o: operations.h
$(BUILD_DIR)/task2.1.o: operations.h
$(BUILD_DIR)/test_operations.o: operations.h
