CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin

SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client

SRCS = msgq.c server.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BIN_DIR)/%: client.c msgq.c
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/msgq.o: msgq.h
$(BUILD_DIR)/client.o: msgq.h
$(BUILD_DIR)/server.o: msgq.h

.PHONY: all clean
