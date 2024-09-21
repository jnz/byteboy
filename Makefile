# Compiler
CC = gcc

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Flags
CFLAGS = -Wall -I$(INCLUDE_DIR) -g
LDFLAGS = -lm

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Output binary
TARGET = $(BUILD_DIR)/byteboy

# Default target
all: $(TARGET)

# Rule to link object files and create the binary
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile .c files into .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean target to remove compiled files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean

