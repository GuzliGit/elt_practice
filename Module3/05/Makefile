CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/task05

SRCS = counter.c task05.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/counter.o: counter.h
$(BUILD_DIR)/task05.o: counter.h

.PHONY: all clean
