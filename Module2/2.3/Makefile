CC = gcc
CFLAGS = -I.
LDFLAGS = -lm

BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/task2.3
TEST_TARGET = $(BUILD_DIR)/test_calc

SRCS = calc.c lexer.c parser.c stack.c task2.3.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET) test

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(TEST_TARGET): $(BUILD_DIR)/calc.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/test_calc.o
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS) $(shell pkg-config --libs check)

test: $(TEST_TARGET)
	@$(TEST_TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(BUILD_DIR)/calc.o: calc.h lexer.h parser.h
$(BUILD_DIR)/lexer.o: lexer.h
$(BUILD_DIR)/parser.o: parser.h lexer.h stack.h
$(BUILD_DIR)/stack.o: stack.h
$(BUILD_DIR)/task2.3.o: calc.h
$(BUILD_DIR)/test_calc.o: calc.h