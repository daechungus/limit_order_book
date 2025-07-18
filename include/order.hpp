#pragma once

#include <cstdint>
#include <string>
#include <iostream>

/**
 * @brief Represents a limit order in the order book
 * 
 * Memory layout considerations:
 * - 64-bit alignment for cache efficiency
 * - Packed tightly to minimize cache misses
 * - id is first for quick lookups
 */
struct Order {
    uint64_t id;        // 8 bytes - unique order identifier
    double price;       // 8 bytes - limit price
    uint32_t quantity;  // 4 bytes - order quantity
    uint32_t side;      // 4 bytes - 0=buy, 1=sell (could use bool but keeping 32-bit for alignment)
    
    // Total: 24 bytes, nicely aligned for 64-bit systems
    
    Order() = default;
    
    // Constructor with move semantics
    Order(uint64_t order_id, double order_price, uint32_t order_qty, uint32_t order_side)
        : id(order_id), price(order_price), quantity(order_qty), side(order_side) {}
    
    // Copy constructor
    Order(const Order& other) = default;
    
    // Move constructor - more efficient than copy
    Order(Order&& other) noexcept = default;
    
    // Copy assignment
    Order& operator=(const Order& other) = default;
    
    // Move assignment - more efficient than copy
    Order& operator=(Order&& other) noexcept = default;
    
    // Destructor
    ~Order() = default;
    
    // Helper methods
    bool is_buy() const { return side == 0; }
    bool is_sell() const { return side == 1; }
    
    // For debugging and output
    friend std::ostream& operator<<(std::ostream& os, const Order& order) {
        os << "Order{id=" << order.id 
           << ", price=" << order.price 
           << ", qty=" << order.quantity 
           << ", side=" << (order.is_buy() ? "BUY" : "SELL") << "}";
        return os;
    }
};

// Static assertion to ensure our struct is cache-friendly
static_assert(sizeof(Order) == 24, "Order struct should be 24 bytes for optimal cache alignment");
static_assert(alignof(Order) == 8, "Order struct should be 8-byte aligned"); 