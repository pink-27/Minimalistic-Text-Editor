# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -w

# Source files
SRCS = run_editor.cpp TextEditor.cpp Cursor.cpp Line.cpp ControlBuffer.cpp
OBJS = $(SRCS:.cpp=.o)

# Output binary
TARGET = editor

# Default rule: build the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule: removes compiled files
clean:
	rm -f $(OBJS) $(TARGET)
