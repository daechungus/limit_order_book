# Limit Order Manager: Low-Latency Snapshot & Throughput Engine

A modern C++ project designed to teach performance engineering concepts through a simulated limit order book system. This project demonstrates key concepts used in high-frequency trading systems.

Core Concepts:
1. Understanding when and how to use move constructors/assignment
2. Resource management and exception safety
3. Cache-freindly data structures and alignment
4. Identifying performance-critical code paths
5. Designing data structures for CPU cache efficiency

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

Requisites:
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

Performance Features
Memory Layout Optimization
- Order struct: 24 bytes, 8-byte aligned for cache efficiency
- Static assertions: Compile-time validation of memory layout
- Move semantics: Avoid unnecessary copies in hot paths

Container Design
- Primary storage: `std::unordered_map` for O(1) order lookup
- Snapshot cache: `std::vector` of pointers for cache-friendly iteration
- Lazy rebuilding: Only rebuild snapshot cache when needed

Benchmarking
- Microsecond precision: High-resolution timing for performance measurement
- Memory tracking: Monitor allocation patterns and memory usage
- Scalability testing: Test from 10 to 1M+ orders

Launch/Building
1. Debug Build (Development)
```bash
make debug
./limit_order_manager interactive
```

2. Memory Leak Detection
```bash
make memcheck
```

3. Performance Testing
```bash
make perf
```

4. Unit Testing (Optional)
```bash
make test
```


Hot vs Cold Paths
- Hot Path: `add_order()` and `cancel_order()` - called frequently
- Cold Path: `print_snapshot()` - called occasionally
- *Optimization Strategy: Optimize hot paths, cache cold path results

Memory Access Patterns
- Sequential Access: Use `std::vector` for iteration
- Random Access: Use `std::unordered_map` for lookups
- Cache Lines: 64-byte alignment for optimal cache performance

To debug: 

AddressSanitizer
```bash
make debug
./limit_order_manager benchmark 1000
```

Valgrind
```bash
make memcheck
```

Performance Profiling
```bash
make profile
gprof limit_order_manager gmon.out
```


This is a learning project. Feel free to reach out if you know more of 
- Multi-threading with lock-free data structures
- Memory-mapped file I/O
- SIMD optimizations for order matching
- Network protocol support (UDP/TCP)

## 📄 License

This project is for educational purposes. Use at your own risk. 
