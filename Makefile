CXX := clang++ 

SRC_DIR := src
BUILD_DIR := build
TARGET := $(BUILD_DIR)/compiler

CXXFLAGS := -std=c++17 -Wall -Wextra -g -O0 -I$(BUILD_DIR) -I$(SRC_DIR)

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	
$(BUILD_DIR)/sysy.tab.cpp: $(SRC_DIR)/sysy.y | $(BUILD_DIR)
	bison -d -o  $@ $<	

$(BUILD_DIR)/sysy.lex.cpp: $(SRC_DIR)/sysy.l $(BUILD_DIR)/sysy.tab.cpp | $(BUILD_DIR)
	flex -o $@ $<

$(TARGET): $(SRC_DIR)/main.cpp $(SRC_DIR)/AST.cpp \
			$(BUILD_DIR)/sysy.tab.cpp $(BUILD_DIR)/sysy.lex.cpp 
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: all clean

clean:
	rm -rf $(BUILD_DIR)