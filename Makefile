# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# List all source files here
SRCS = main.cpp calculator.cpp
# Automatically generate object file list
OBJS = $(SRCS:.cpp=.o)

# The target executable name
TARGET = main

# The default target that compiles everything
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Pattern rule: compile any .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
