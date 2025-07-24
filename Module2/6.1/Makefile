CC = gcc
CFLAGS = -I.

AR = ar
ARFLAGS = rc

BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

TARGET = $(BIN_DIR)/task6.1
TEST_TARGET = $(BIN_DIR)/test_contact_list
LIB_TARGET = $(LIB_DIR)/libcontactlist.a

all: $(TARGET) test

$(TARGET): $(BUILD_DIR)/task6.1.o $(BUILD_DIR)/contact_api.o $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ -L$(LIB_DIR) -lcontactlist

$(TEST_TARGET): $(BUILD_DIR)/test_contact_list.o $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ -L$(LIB_DIR) $(shell pkg-config --libs check) -lcontactlist

$(LIB_TARGET): $(BUILD_DIR)/contact_list.o
	@mkdir -p $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^

test: $(TEST_TARGET)
	@$(TEST_TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/contact_api.o: contact_api.h contact_list.h
$(BUILD_DIR)/contact_list.o: contact_list.h
$(BUILD_DIR)/task6.1.o: contact_list.h contact_api.h
$(BUILD_DIR)/test_contact_list.o: contact_list.h
