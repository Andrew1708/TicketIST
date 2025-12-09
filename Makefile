# Compiler settings
CXX = g++
CXXFLAGS = -Wall -std=c++17

# Linker settings
LDFLAGS = -lssl -lcrypto -lmysqlcppconn 

# Directories
COMMON_DIR = common
TEST_DIR = test
INCLUDE_DIR = $(COMMON_DIR)/include
SRC_DIR = $(COMMON_DIR)/src
TEST_INCLUDE_DIR = $(TEST_DIR)/include
TEST_SRC_DIR = $(TEST_DIR)/src

# Explicitly specify subdirectories under include
INCLUDE_SUBDIRS = $(INCLUDE_DIR)/User $(INCLUDE_DIR)/Crypto $(INCLUDE_DIR)/Ticket $(INCLUDE_DIR)/Message $(INCLUDE_DIR)/Communication $(INCLUDE_DIR)/Repositories $(INCLUDE_DIR)/Services $(INCLUDE_DIR)/Event $(INCLUDE_DIR)/Controller

# Find all .cpp files (recursively)
COMMON_SRC = $(shell find $(SRC_DIR) -name "*.cpp")
COMMON_OBJ = $(COMMON_SRC:$(SRC_DIR)/%.cpp=$(SRC_DIR)/%.o)

TEST_SRC = $(shell find $(TEST_SRC_DIR) -name "*.cpp")
TEST_OBJ = $(TEST_SRC:$(TEST_SRC_DIR)/%.cpp=$(TEST_DIR)/%.o)

# Rule for compiling common .cpp files to .o files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCLUDE_SUBDIRS)) -c $< -o $@

# Rule for compiling test .cpp files to their corresponding test executables
$(TEST_DIR)/%.exe: $(TEST_SRC_DIR)/%.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCLUDE_SUBDIRS)) -I$(INCLUDE_DIR) -I$(TEST_INCLUDE_DIR) $< $(COMMON_OBJ) $(LDFLAGS) -o $@

# Define TEST_TARGETS with .exe extension
TEST_TARGETS = $(TEST_SRC:$(TEST_SRC_DIR)/%.cpp=$(TEST_DIR)/%.exe)

# Additional target for Server, Client, and GenerateServerKeys executables with .exe extension
SERVER_EXEC = Server.exe
CLIENT_EXEC = Client.exe
VALIDATOR_EXEC = Validator.exe
GENERATE_SERVER_KEYS_EXEC = GenerateServerKeys.exe

# Default target for compiling tests and the server/client executables
all: $(TEST_TARGETS) $(SERVER_EXEC) $(CLIENT_EXEC) $(VALIDATOR_EXEC) $(GENERATE_SERVER_KEYS_EXEC)

# Rule for compiling Server.cpp and Client.cpp with .exe extension
$(SERVER_EXEC): Server.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCLUDE_SUBDIRS)) -I$(INCLUDE_DIR) -I$(TEST_INCLUDE_DIR) $< $(COMMON_OBJ) $(LDFLAGS) -o $@

$(CLIENT_EXEC): Client.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCLUDE_SUBDIRS)) -I$(INCLUDE_DIR) -I$(TEST_INCLUDE_DIR) $< $(COMMON_OBJ) $(LDFLAGS) -o $@

$(VALIDATOR_EXEC): Validator.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCLUDE_SUBDIRS)) -I$(INCLUDE_DIR) -I$(TEST_INCLUDE_DIR) $< $(COMMON_OBJ) $(LDFLAGS) -o $@

# Rule for compiling GenerateServerKeys.cpp with .exe extension
$(GENERATE_SERVER_KEYS_EXEC): GenerateServerKeys.cpp $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) $(addprefix -I, $(INCLUDE_SUBDIRS)) -I$(INCLUDE_DIR) -I$(TEST_INCLUDE_DIR) $< $(COMMON_OBJ) $(LDFLAGS) -o $@

# Clean the build files
clean:
	rm -f $(COMMON_OBJ) $(TEST_OBJ) $(TEST_TARGETS) $(SERVER_EXEC) $(CLIENT_EXEC) $(GENERATE_SERVER_KEYS_EXEC)

.PHONY: all clean
