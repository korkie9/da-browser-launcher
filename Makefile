# Compiler
CC = g++

# Compiler flags
CXXFLAGS ?= -Wall -std=c++17

# Libraries
LDLIBS   += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lsqlite3

# Paths
SRC = main.cpp utils/db/db.cpp utils/browser-menu/browser-menu.cpp utils/get-resource-path/get-resource-path.cpp
OBJ = $(SRC:.cpp=.o)
CONFIG_DIR = $(HOME)/.config/da-browser-launcher
RESOURCE_DIR = resources
DB = $(CONFIG_DIR)/database.db

# Target executable
TARGET = da_browser_launcher

all: | $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@

# Compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(DB)

install-resources:
	mkdir -p ~/.local/share/da-browser-launcher/
	cp -r assets/* ~/.local/share/da-browser-launcher/
