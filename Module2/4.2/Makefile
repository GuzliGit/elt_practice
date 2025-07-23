CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/task4.2

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/p_queue.o $(BUILD_DIR)/task4.2.o
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/p_queue.o: p_queue.h
$(BUILD_DIR)/task4.2.o: p_queue.h
