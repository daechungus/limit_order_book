#pragma once

#include "order.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

/**
 * @brief Manages a collection of active orders
 * 
 * Performance considerations:
 * - unordered_map for O(1) order lookup by ID
 * - vector for snapshot printing (cache-friendly iteration)
 * - Reserve capacity to avoid reallocations
 * - Use move semantics to avoid copies
 */
class OrderManager {
private:
    // Primary storage: O(1) lookup by order ID
    // This is the "hot path" - accessed on every add/cancel
    std::unordered_map<uint64_t, Order> orders_;
    
    // Secondary storage: cache-friendly for snapshot printing
    // This is the "cold path" - only accessed when printing
    mutable std::vector<const Order*> order_ptrs_;
    mutable bool snapshot_dirty_ = true;
    
    // Performance tracking
    uint64_t total_orders_added_ = 0;
    uint64_t total_orders_cancelled_ = 0;
    
public:
    OrderManager() = default;
    ~OrderManager() = default;
    
    // Disable copying - we want explicit control over memory
    OrderManager(const OrderManager&) = delete;
    OrderManager& operator=(const OrderManager&) = delete;
    
    // Allow moving - useful for transferring ownership
    OrderManager(OrderManager&&) = default;
    OrderManager& operator=(OrderManager&&) = default;
    
    /**
     * @brief Add a new order to the manager
     * @param order The order to add (will be moved)
     * @return true if added successfully, false if ID already exists
     */
    bool add_order(Order order);
    
    /**
     * @brief Cancel an order by ID
     * @param order_id The ID of the order to cancel
     * @return true if cancelled successfully, false if not found
     */
    bool cancel_order(uint64_t order_id);
    
    /**
     * @brief Get an order by ID (const access)
     * @param order_id The ID to look up
     * @return Pointer to order if found, nullptr otherwise
     */
    const Order* get_order(uint64_t order_id) const;
    
    /**
     * @brief Print a snapshot of all active orders
     * @param os Output stream (default: std::cout)
     */
    void print_snapshot(std::ostream& os = std::cout) const;
    
    /**
     * @brief Print a snapshot to a file
     * @param filename The file to write to
     */
    void print_snapshot_to_file(const std::string& filename) const;
    
    /**
     * @brief Load orders from a CSV file
     * @param filename The CSV file to read from
     * @return Number of orders successfully loaded
     */
    size_t load_from_csv(const std::string& filename);
    
    /**
     * @brief Get statistics about the order manager
     */
    void print_stats(std::ostream& os = std::cout) const;
    
    /**
     * @brief Get the number of active orders
     */
    size_t size() const { return orders_.size(); }
    
    /**
     * @brief Check if the manager is empty
     */
    bool empty() const { return orders_.empty(); }
    
    /**
     * @brief Clear all orders
     */
    void clear();
    
private:
    /**
     * @brief Rebuild the snapshot cache if dirty
     * This is called automatically when needed
     */
    void rebuild_snapshot_cache() const;
    
    /**
     * @brief Parse a CSV line into an Order
     * @param line The CSV line to parse
     * @return Parsed order or nullptr if invalid
     */
    std::unique_ptr<Order> parse_csv_line(const std::string& line) const;
}; 