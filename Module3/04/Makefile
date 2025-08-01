CC = gcc
CFLAGS = -I.

PROGS_DIR = mini_progs
BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/task04
PROGS_TARGETS = $(BIN_DIR)/find_long $(BIN_DIR)/invert $(BIN_DIR)/sum

SRCS = com_interpreter.c interpreter_api.c com_parser.c task04.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET) $(PROGS_TARGETS)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(BIN_DIR)/%: $(PROGS_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/com_interpreter.o: com_interpreter.h
$(BUILD_DIR)/interpreter_api.o: com_interpreter.h com_parser.h
$(BUILD_DIR)/com_parser.o: com_parser.h com_interpreter.h
$(BUILD_DIR)/task04.o: interpreter_api.h

.PHONY: all clean
