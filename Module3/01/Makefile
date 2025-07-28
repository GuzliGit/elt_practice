CC = gcc
CFLAGS = -I.
LDFLAGS = -lm

BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/task01

SRCS = arg_converter.c task01.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/arg_converter.o: arg_converter.h
$(BUILD_DIR)/task01.o: arg_converter.h

.PHONY: all clean
