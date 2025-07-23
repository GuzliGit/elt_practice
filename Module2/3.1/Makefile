CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/task3.1

SRCS = permissions.c task3.1.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILD_DIR)/task3.1.o: permissions.h
$(BUILD_DIR)/permissions.o: permissions.h
