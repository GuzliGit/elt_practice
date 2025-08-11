CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin

SERVER_TARGET = $(BIN_DIR)/e_server
CLIENT_TARGET = $(BIN_DIR)/e_client

SERVER_SRC = e_server.c client_list.c packets.c
CLIENT_SRC = e_client.c packets.c

SERVER_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(CLIENT_SRC))

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

$(CLIENT_TARGET): $(CLIENT_OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/e_server.o: client_list.h packets.h
$(BUILD_DIR)/client_list.o: client_list.h
$(BUILD_DIR)/packets.o: packets.h
$(BUILD_DIR)/e_client.o: packets.h

.PHONY: all clean
