CC = gcc
CFLAGS = -I. -fPIC
LDFLAGS = -lm -ldl

BUILD_DIR = build
BIN_DIR = bin
LIBS_DIR = operations
OPERATIONS = sum substraction multiply divide unary_minus

TARGET = $(BIN_DIR)/task6.3

SRCS = calc.c lexer.c parser.c stack.c task6.3.c
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

rebuild_%: $(LIBS_DIR)/%.so
	@echo "$*.so rebuilded"

all: $(TARGET) $(patsubst %,$(LIBS_DIR)/%.so,$(OPERATIONS))

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(LIBS_DIR)/%.so: operations/%.c
	@mkdir -p $(LIBS_DIR)
	$(CC) $(CFLAGS) -shared $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/calc.o: calc.h lexer.h parser.h
$(BUILD_DIR)/lexer.o: lexer.h
$(BUILD_DIR)/parser.o: parser.h lexer.h stack.h
$(BUILD_DIR)/stack.o: stack.h
$(BUILD_DIR)/task6.3.o: calc.h