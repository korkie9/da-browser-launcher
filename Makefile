# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17

# Libraries
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lsqlite3

# Paths
SRC = main.cpp utils/db/db.cpp utils/browser-menu/browser-menu.cpp utils/get-resource-path/get-resource-path.cpp
OBJ = $(SRC:.cpp=.o)
CONFIG_DIR = $(HOME)/.config/da-browser-launcher
RESOURCE_DIR = resources
DB = $(CONFIG_DIR)/database.db

# Target executable
TARGET = da_browser_launcher

all: | $(CONFIG_DIR) $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(DB)

install-resources:
	mkdir -p ~/.local/share/da-browser-launcher/
	cp -r assets/* ~/.local/share/da-browser-launcher/
