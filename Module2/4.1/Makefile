CC = gcc
CFLAGS = -I.

BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/task4.1
TEST_TARGET = $(BIN_DIR)/test_contact_list

all: $(TARGET) test

$(TARGET): $(BUILD_DIR)/contact_list.o $(BUILD_DIR)/task4.1.o $(BUILD_DIR)/contact_api.o
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@

$(TEST_TARGET): $(BUILD_DIR)/contact_list.o $(BUILD_DIR)/test_contact_list.o
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(shell pkg-config --libs check)

test: $(TEST_TARGET)
	@$(TEST_TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/contact_api.o: contact_api.h contact_list.h
$(BUILD_DIR)/contact_list.o: contact_list.h
$(BUILD_DIR)/task4.1.o: contact_list.h contact_api.h
$(BUILD_DIR)/test_contact_list.o: contact_list.h
