#include "../include/order_manager.hpp"
#include <cassert>
#include <iostream>

// Simple test framework
#define TEST(name) void test_##name()
#define ASSERT(condition) assert(condition)
#define RUN_TEST(name) { std::cout << "Running " #name "... "; test_##name(); std::cout << "PASSED" << std::endl; }

// Test Order struct
TEST(order_creation) {
    Order order(1, 150.50, 100, 0);
    ASSERT(order.id == 1);
    ASSERT(order.price == 150.50);
    ASSERT(order.quantity == 100);
    ASSERT(order.side == 0);
    ASSERT(order.is_buy());
    ASSERT(!order.is_sell());
}

TEST(order_move_semantics) {
    Order original(1, 150.50, 100, 0);
    Order moved = std::move(original);
    
    ASSERT(moved.id == 1);
    ASSERT(moved.price == 150.50);
    ASSERT(moved.quantity == 100);
    ASSERT(moved.side == 0);
}

TEST(order_memory_layout) {
    ASSERT(sizeof(Order) == 24);
    ASSERT(alignof(Order) == 8);
}

// Test OrderManager
TEST(ordermanager_basic) {
    OrderManager manager;
    ASSERT(manager.empty());
    ASSERT(manager.size() == 0);
}

TEST(ordermanager_add_orders) {
    OrderManager manager;
    
    Order order1(1, 150.50, 100, 0);
    Order order2(2, 151.25, 200, 1);
    
    ASSERT(manager.add_order(std::move(order1)));
    ASSERT(manager.add_order(std::move(order2)));
    ASSERT(!manager.add_order(Order(1, 160.00, 50, 0))); // Duplicate ID
    
    ASSERT(manager.size() == 2);
}

TEST(ordermanager_cancel_orders) {
    OrderManager manager;
    
    manager.add_order(Order(1, 150.50, 100, 0));
    manager.add_order(Order(2, 151.25, 200, 1));
    
    ASSERT(manager.cancel_order(1));
    ASSERT(!manager.cancel_order(1)); // Already cancelled
    ASSERT(!manager.cancel_order(999)); // Non-existent
    
    ASSERT(manager.size() == 1);
}

TEST(ordermanager_get_order) {
    OrderManager manager;
    
    Order order(1, 150.50, 100, 0);
    manager.add_order(std::move(order));
    
    const Order* found = manager.get_order(1);
    ASSERT(found != nullptr);
    ASSERT(found->id == 1);
    ASSERT(found->price == 150.50);
    
    const Order* not_found = manager.get_order(999);
    ASSERT(not_found == nullptr);
}

TEST(ordermanager_clear) {
    OrderManager manager;
    
    manager.add_order(Order(1, 150.50, 100, 0));
    manager.add_order(Order(2, 151.25, 200, 1));
    
    ASSERT(manager.size() == 2);
    
    manager.clear();
    ASSERT(manager.empty());
    ASSERT(manager.size() == 0);
}

// Test CSV parsing
TEST(csv_parsing) {
    OrderManager manager;
    
    // Create a temporary CSV file
    std::ofstream temp_file("temp_test.csv");
    temp_file << "id,price,quantity,side\n";
    temp_file << "1,150.50,100,0\n";
    temp_file << "2,151.25,200,1\n";
    temp_file.close();
    
    size_t loaded = manager.load_from_csv("temp_test.csv");
    ASSERT(loaded == 2);
    ASSERT(manager.size() == 2);
    
    // Clean up
    std::remove("temp_test.csv");
}

int main() {
    std::cout << "Running Limit Order Manager Tests..." << std::endl;
    std::cout << "=====================================" << std::endl;
    
    RUN_TEST(order_creation);
    RUN_TEST(order_move_semantics);
    RUN_TEST(order_memory_layout);
    RUN_TEST(ordermanager_basic);
    RUN_TEST(ordermanager_add_orders);
    RUN_TEST(ordermanager_cancel_orders);
    RUN_TEST(ordermanager_get_order);
    RUN_TEST(ordermanager_clear);
    RUN_TEST(csv_parsing);
    
    std::cout << std::endl;
    std::cout << "All tests passed!" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    return 0;
} 