CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

SRC_DIR := .
BUILD_DIR := build
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

TARGET := $(BIN_DIR)/main

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

all: $(BUILD_DIR) $(BIN_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


run: all
	./$(TARGET)
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Regla phony para evitar conflictos
.PHONY: all clean run
