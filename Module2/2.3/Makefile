CC = gcc
CFLAGS = -I.
LDFLAGS = -lm

BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/task2.3

SRCS = calc.c lexer.c parser.c stack.c task2.3.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/calc.o: calc.h lexer.h parsec.h
$(BUILD_DIR)/lexer.o: lexer.h
$(BUILD_DIR)/parsec.o: parsec.h lexer.h stack.h
$(BUILD_DIR)/stack.o: stack.h
$(BUILD_DIR)/task2.3.o: calc.h