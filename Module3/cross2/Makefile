CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/taxi_manager

SRC = taxi_manager.c drivers_manager.c driver.c

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

$(BUILD_DIR)/taxi_manager.o: drivers_manager.h
$(BUILD_DIR)/drivers_manager.o: drivers_manager.h driver.h
$(BUILD_DIR)/driver.o: driver.h

.PHONY: all clean
