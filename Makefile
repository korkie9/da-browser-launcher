# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17

# Libraries
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lsqlite3

# Paths
SRC = main.cpp utils/db.cpp utils/browser-menu.cpp
OBJ = $(SRC:.cpp=.o)
DB=database.db

# Target executable
TARGET = da_browser_launcher

all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(DB)
