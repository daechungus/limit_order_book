# Limit Order Manager: Low-Latency Snapshot & Throughput Engine

A modern C++ project designed to teach performance engineering concepts through a simulated limit order book system. This project demonstrates key concepts used in high-frequency trading systems.

## 🎯 Learning Objectives

### C++ Core Concepts
- **Move Semantics**: Understanding when and how to use move constructors/assignment
- **RAII**: Resource management and exception safety
- **Modern C++**: C++17/20 features like structured bindings, auto, and smart pointers
- **Memory Layout**: Cache-friendly data structures and alignment

### Performance Engineering
- **Hot vs Cold Paths**: Identifying performance-critical code paths
- **Container Choices**: When to use `std::unordered_map` vs `std::vector`
- **Memory Allocation**: Avoiding unnecessary allocations and copies
- **Cache Locality**: Designing data structures for CPU cache efficiency

### Tools & Debugging
- **Sanitizers**: AddressSanitizer and UndefinedBehaviorSanitizer
- **Valgrind**: Memory leak detection and profiling
- **Performance Profiling**: Measuring and optimizing code performance
- **Benchmarking**: Systematic performance testing

## 🏗️ Project Structure

```
limit_order_project/
├── include/
│   ├── order.hpp          # Order struct with cache-friendly layout
│   └── order_manager.hpp  # OrderManager class with performance optimizations
├── src/
│   ├── main.cpp           # Main program with CLI and benchmarking
│   └── order_manager.cpp  # OrderManager implementation
├── data/
│   └── ticks.txt          # Sample order data
├── scripts/
│   ├── gen_orders.py      # Order generator (Python)
│   └── bench.sh           # Benchmarking script
├── tests/
│   └── order_test.cpp     # Unit tests (optional)
├── Makefile               # Build system with multiple configurations
└── README.md              # This file
```

## 🚀 Quick Start

### Prerequisites
- GCC 7+ or Clang 6+ with C++17 support
- Make
- Python 3 (for order generation script)

### Building
```bash
# Debug build with sanitizers (recommended for development)
make debug

# Release build with optimizations (for performance testing)
make release

# Profile build with profiling info
make profile
```

### Running
```bash
# Load sample data and print snapshot
./limit_order_manager load data/ticks.txt

# Generate 1000 random orders
./limit_order_manager generate 1000

# Run performance benchmark with 10,000 orders
./limit_order_manager benchmark 10000

# Start interactive mode
./limit_order_manager interactive

# Print statistics
./limit_order_manager stats
```

## 📊 Performance Features

### Memory Layout Optimization
- **Order struct**: 24 bytes, 8-byte aligned for cache efficiency
- **Static assertions**: Compile-time validation of memory layout
- **Move semantics**: Avoid unnecessary copies in hot paths

### Container Design
- **Primary storage**: `std::unordered_map` for O(1) order lookup
- **Snapshot cache**: `std::vector` of pointers for cache-friendly iteration
- **Lazy rebuilding**: Only rebuild snapshot cache when needed

### Benchmarking
- **Microsecond precision**: High-resolution timing for performance measurement
- **Memory tracking**: Monitor allocation patterns and memory usage
- **Scalability testing**: Test from 10 to 1M+ orders

## 🔧 Development Workflow

### 1. Debug Build (Development)
```bash
make debug
./limit_order_manager interactive
```

### 2. Memory Leak Detection
```bash
make memcheck
```

### 3. Performance Testing
```bash
make perf
```

### 4. Unit Testing (Optional)
```bash
make test
```

## 🧠 Key Performance Concepts

### Hot vs Cold Paths
- **Hot Path**: `add_order()` and `cancel_order()` - called frequently
- **Cold Path**: `print_snapshot()` - called occasionally
- **Optimization Strategy**: Optimize hot paths, cache cold path results

### Memory Access Patterns
- **Sequential Access**: Use `std::vector` for iteration
- **Random Access**: Use `std::unordered_map` for lookups
- **Cache Lines**: 64-byte alignment for optimal cache performance

### Move vs Copy
- **Move Semantics**: Transfer ownership without copying data
- **Copy Elision**: Compiler optimization to avoid unnecessary copies
- **Perfect Forwarding**: Preserve value categories in templates

## 📈 Scaling Considerations

### Current Limitations
- Single-threaded design
- In-memory storage only
- No persistence or recovery

### Future Enhancements
- Multi-threading with lock-free data structures
- Memory-mapped file I/O
- SIMD optimizations for order matching
- Network protocol support (UDP/TCP)

## 🛠️ Debugging Tips

### AddressSanitizer
```bash
make debug
./limit_order_manager benchmark 1000
# ASan will catch buffer overflows, use-after-free, etc.
```

### Valgrind
```bash
make memcheck
# Valgrind will detect memory leaks and invalid memory access
```

### Performance Profiling
```bash
make profile
gprof limit_order_manager gmon.out
# Profile to identify performance bottlenecks
```

## 🎓 Quant PM Style Questions

As you work through this project, consider these questions:

1. **Memory Layout**: Why is the Order struct 24 bytes? Could we make it smaller?
2. **Container Choice**: When would we choose `std::map` over `std::unordered_map`?
3. **Hot Path Optimization**: Which functions are called most frequently? How can we optimize them?
4. **Cache Efficiency**: How does our data layout affect CPU cache performance?
5. **Scalability**: What happens when we scale to 1M orders? What breaks first?

## 📚 Further Reading

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/)
- [Systems Performance](https://www.brendangregg.com/systems-performance-book.html)
- [High Performance Trading Systems](https://www.amazon.com/High-Performance-Trading-Systems-Architecture/dp/0470128015)

## 🤝 Contributing

This is a learning project. Feel free to:
- Add new performance optimizations
- Implement additional features
- Improve the benchmarking system
- Add more comprehensive tests

## 📄 License

This project is for educational purposes. Use at your own risk. 