CC = gcc
CFLAGS = -I.
LDFLAGS = -lm

BUILD_DIR = build
BIN_DIR = bin
CALC_DIR = calc

SERVER_TARGET = $(BIN_DIR)/tcp_server
CLIENT_TARGET = $(BIN_DIR)/tcp_client

SERVER_SRC = tcp_server.c $(CALC_DIR)/calc.c $(CALC_DIR)/lexer.c $(CALC_DIR)/parser.c $(CALC_DIR)/stack.c   
CLIENT_SRC = tcp_client.c

SERVER_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SERVER_SRC))
CLIENT_OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(CLIENT_SRC))

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CLIENT_TARGET): $(CLIENT_OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/tcp_server.o: $(CALC_DIR)/calc.h $(CALC_DIR)/lexer.h $(CALC_DIR)/parser.h
$(BUILD_DIR)/calc/calc.o: $(CALC_DIR)/calc.h $(CALC_DIR)/lexer.h $(CALC_DIR)/parser.h
$(BUILD_DIR)/calc/lexer.o: $(CALC_DIR)/lexer.h
$(BUILD_DIR)/calc/parser.o: $(CALC_DIR)/parser.h $(CALC_DIR)/lexer.h $(CALC_DIR)/stack.h
$(BUILD_DIR)/calc/stack.o: $(CALC_DIR)/stack.h

.PHONY: all clean
