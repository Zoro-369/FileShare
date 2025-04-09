CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -Iinclude -g
SRC_DIR = src
BUILD_DIR = build
BIN = snapdrop

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

$(BIN): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(OBJ_FILES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN)

run: $(BIN)
	./$(BIN) 



