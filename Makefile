# Limit Order Manager Makefile
# Supports different build configurations for development and performance

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
INCLUDES = -Iinclude
SOURCES = src/main.cpp src/order_manager.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = limit_order_manager

# Build configurations
.PHONY: all debug release profile clean test

# Default build (debug with sanitizers)
all: debug

# Debug build with sanitizers and debug info
debug: CXXFLAGS += -g -O0 -fsanitize=address -fsanitize=undefined
debug: $(TARGET)

# Release build with optimizations
release: CXXFLAGS += -O3 -DNDEBUG -march=native
release: $(TARGET)

# Profile build with optimization and profiling info
profile: CXXFLAGS += -O2 -g -pg
profile: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) *.out gmon.out

# Run tests
test: debug
	@echo "Running basic functionality tests..."
	./$(TARGET) load data/ticks.txt
	@echo "Running performance benchmark..."
	./$(TARGET) benchmark 1000

# Performance testing
perf: release
	@echo "Running performance tests..."
	./$(TARGET) benchmark 10000

# Memory leak detection (requires debug build)
memcheck: debug
	@echo "Running memory leak detection..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) benchmark 1000

# Interactive mode
interactive: debug
	./$(TARGET) interactive

# Generate sample data
generate-data:
	@echo "Generating sample order data..."
	./$(TARGET) generate 100 > data/generated_orders.txt

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build debug version (default)"
	@echo "  debug        - Build with debug info and sanitizers"
	@echo "  release      - Build optimized version"
	@echo "  profile      - Build with profiling info"
	@echo "  test         - Run basic tests"
	@echo "  perf         - Run performance tests"
	@echo "  memcheck     - Run memory leak detection"
	@echo "  interactive  - Start interactive mode"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help" 