CC = gcc
CFLAGS = -I. -pthread

BUILD_DIR = build
BIN_DIR = bin

SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): server.c
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(CLIENT_TARGET): client.c
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
