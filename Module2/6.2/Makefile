CC = gcc
CFLAGS = -I. -fPIC
LDFLAGS = -L$(LIB_DIR) -lcontactlist -Wl,-rpath,'$$ORIGIN/../lib'

BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = lib

TARGET = $(BIN_DIR)/task6.2
TEST_TARGET = $(BIN_DIR)/test_contact_list
LIB_TARGET = $(LIB_DIR)/libcontactlist.so

all: $(TARGET) test

$(TARGET): $(BUILD_DIR)/task6.2.o $(BUILD_DIR)/contact_api.o $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(TEST_TARGET): $(BUILD_DIR)/test_contact_list.o $(LIB_TARGET)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS) $(shell pkg-config --libs check)

$(LIB_TARGET): $(BUILD_DIR)/contact_list.o
	@mkdir -p $(LIB_DIR)
	$(CC) -shared $^ -o $@

test: $(TEST_TARGET)
	@LD_LIBRARY_PATH=$(LIB_DIR) $(TEST_TARGET)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@LD_LIBRARY_PATH=$(LIB_DIR) $(TARGET)

$(BUILD_DIR)/contact_api.o: contact_api.h contact_list.h
$(BUILD_DIR)/contact_list.o: contact_list.h
$(BUILD_DIR)/task6.2.o: contact_list.h contact_api.h
$(BUILD_DIR)/test_contact_list.o: contact_list.h
