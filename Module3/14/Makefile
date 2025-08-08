CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/vor

SRC = vor.c

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

all: $(TARGET) 

$(TARGET): $(OBJ)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
