# Time Anchor - Makefile
# SENG479 Game Programming

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Raylib settings (adjust paths as needed)
# For Windows with MinGW:
RAYLIB_PATH = C:/raylib/raylib

# Include and library paths
INCLUDES = -I$(RAYLIB_PATH)/src -Isrc
LIBS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm

# For Linux, uncomment these:
# LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Source files
SRC_DIR = src
BUILD_DIR = build
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Output
TARGET = $(BUILD_DIR)/TimeAnchor.exe
# For Linux: TARGET = $(BUILD_DIR)/TimeAnchor

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# Compile
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Run
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run
