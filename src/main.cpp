#include "../include/order_manager.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <string>

// Performance measurement utilities
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_;
    std::string name_;
    
public:
    Timer(const std::string& name) : name_(name) {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        std::cout << name_ << " took " << duration.count() << " microseconds" << std::endl;
    }
};

// Generate random orders for performance testing
void generate_random_orders(OrderManager& manager, size_t count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> price_dist(100.0, 200.0);
    std::uniform_int_distribution<uint32_t> qty_dist(1, 1000);
    std::uniform_int_distribution<uint32_t> side_dist(0, 1);
    
    std::cout << "Generating " << count << " random orders..." << std::endl;
    
    {
        Timer timer("Order generation");
        for (size_t i = 0; i < count; ++i) {
            Order order(i + 1, price_dist(gen), qty_dist(gen), side_dist(gen));
            manager.add_order(std::move(order));
        }
    }
    
    std::cout << "Generated " << count << " orders successfully." << std::endl;
}

// Performance benchmark
void run_benchmark(OrderManager& manager, size_t order_count) {
    std::cout << "\n=== PERFORMANCE BENCHMARK ===" << std::endl;
    std::cout << "Testing with " << order_count << " orders" << std::endl;
    
    // Clear existing orders
    manager.clear();
    
    // Benchmark order addition
    {
        Timer timer("Order addition");
        generate_random_orders(manager, order_count);
    }
    
    // Benchmark snapshot printing
    {
        Timer timer("Snapshot printing");
        manager.print_snapshot();
    }
    
    // Benchmark order cancellation
    {
        Timer timer("Order cancellation");
        for (size_t i = 1; i <= std::min(order_count, size_t(1000)); ++i) {
            manager.cancel_order(i);
        }
    }
    
    // Print final stats
    manager.print_stats();
}

void print_usage() {
    std::cout << "Limit Order Manager - Performance Testing Tool" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./main [command] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  load <filename>     - Load orders from CSV file" << std::endl;
    std::cout << "  generate <count>    - Generate random orders" << std::endl;
    std::cout << "  benchmark <count>   - Run performance benchmark" << std::endl;
    std::cout << "  snapshot [filename] - Print snapshot (optional file output)" << std::endl;
    std::cout << "  stats              - Print statistics" << std::endl;
    std::cout << "  interactive        - Start interactive mode" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./main load data/ticks.txt" << std::endl;
    std::cout << "  ./main generate 1000" << std::endl;
    std::cout << "  ./main benchmark 10000" << std::endl;
    std::cout << "  ./main snapshot output.txt" << std::endl;
}

void interactive_mode(OrderManager& manager) {
    std::cout << "Interactive Mode - Type 'help' for commands" << std::endl;
    
    std::string command;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, command);
        
        if (command == "quit" || command == "exit") {
            break;
        } else if (command == "help") {
            std::cout << "Commands: add, cancel, snapshot, stats, clear, quit" << std::endl;
        } else if (command == "snapshot") {
            manager.print_snapshot();
        } else if (command == "stats") {
            manager.print_stats();
        } else if (command == "clear") {
            manager.clear();
            std::cout << "All orders cleared." << std::endl;
        } else if (command.substr(0, 3) == "add") {
            // Format: add <id> <price> <qty> <side>
            std::istringstream iss(command);
            std::string cmd;
            uint64_t id;
            double price;
            uint32_t qty, side;
            
            if (iss >> cmd >> id >> price >> qty >> side) {
                Order order(id, price, qty, side);
                if (manager.add_order(std::move(order))) {
                    std::cout << "Order added successfully." << std::endl;
                } else {
                    std::cout << "Failed to add order (ID already exists)." << std::endl;
                }
            } else {
                std::cout << "Usage: add <id> <price> <qty> <side>" << std::endl;
            }
        } else if (command.substr(0, 6) == "cancel") {
            // Format: cancel <id>
            std::istringstream iss(command);
            std::string cmd;
            uint64_t id;
            
            if (iss >> cmd >> id) {
                if (manager.cancel_order(id)) {
                    std::cout << "Order cancelled successfully." << std::endl;
                } else {
                    std::cout << "Order not found." << std::endl;
                }
            } else {
                std::cout << "Usage: cancel <id>" << std::endl;
            }
        } else {
            std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    OrderManager manager;
    
    if (argc < 2) {
        print_usage();
        return 0;
    }
    
    std::string command = argv[1];
    
    try {
        if (command == "load" && argc >= 3) {
            std::string filename = argv[2];
            std::cout << "Loading orders from " << filename << "..." << std::endl;
            
            Timer timer("CSV loading");
            size_t loaded = manager.load_from_csv(filename);
            std::cout << "Loaded " << loaded << " orders." << std::endl;
            
            manager.print_snapshot();
            
        } else if (command == "generate" && argc >= 3) {
            size_t count = std::stoul(argv[2]);
            generate_random_orders(manager, count);
            manager.print_snapshot();
            
        } else if (command == "benchmark" && argc >= 3) {
            size_t count = std::stoul(argv[2]);
            run_benchmark(manager, count);
            
        } else if (command == "snapshot") {
            if (argc >= 3) {
                std::string filename = argv[2];
                manager.print_snapshot_to_file(filename);
                std::cout << "Snapshot saved to " << filename << std::endl;
            } else {
                manager.print_snapshot();
            }
            
        } else if (command == "stats") {
            manager.print_stats();
            
        } else if (command == "interactive") {
            interactive_mode(manager);
            
        } else {
            print_usage();
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 