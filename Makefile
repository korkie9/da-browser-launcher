# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17

# Libraries
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lsqlite3

# Paths
INCLUDES = -I./libtinyfiledialogs -I./utils
LIBS = ./libtinyfiledialogs/tinyfiledialogs.c
SRC = main.cpp utils/db.cpp $(LIBS)
OBJ = $(SRC:.cpp=.o)

# Target executable
TARGET = da_browser_launcher

all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compilation
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
