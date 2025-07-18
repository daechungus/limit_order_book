#include "../include/order_manager.hpp"
#include <algorithm>
#include <iomanip>
#include <stdexcept>

bool OrderManager::add_order(Order order) {
    // Check if order ID already exists
    auto [it, inserted] = orders_.try_emplace(order.id, std::move(order));
    
    if (inserted) {
        total_orders_added_++;
        snapshot_dirty_ = true;  // Mark snapshot cache as dirty
        return true;
    }
    return false;
}

bool OrderManager::cancel_order(uint64_t order_id) {
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        orders_.erase(it);
        total_orders_cancelled_++;
        snapshot_dirty_ = true;  // Mark snapshot cache as dirty
        return true;
    }
    return false;
}

const Order* OrderManager::get_order(uint64_t order_id) const {
    auto it = orders_.find(order_id);
    return (it != orders_.end()) ? &(it->second) : nullptr;
}

void OrderManager::print_snapshot(std::ostream& os) const {
    rebuild_snapshot_cache();
    
    os << "\n=== ORDER BOOK SNAPSHOT ===" << std::endl;
    os << "Total Active Orders: " << order_ptrs_.size() << std::endl;
    os << std::endl;
    
    if (order_ptrs_.empty()) {
        os << "No active orders." << std::endl;
        return;
    }
    
    // Print header
    os << std::setw(12) << "Order ID" 
       << std::setw(12) << "Price" 
       << std::setw(12) << "Quantity" 
       << std::setw(8) << "Side" << std::endl;
    os << std::string(44, '-') << std::endl;
    
    // Print orders sorted by price (best prices first)
    std::vector<const Order*> sorted_orders = order_ptrs_;
    std::sort(sorted_orders.begin(), sorted_orders.end(), 
              [](const Order* a, const Order* b) {
                  if (a->is_buy() != b->is_buy()) {
                      return a->is_buy();  // Buy orders first
                  }
                  if (a->is_buy()) {
                      return a->price > b->price;  // Higher buy prices first
                  } else {
                      return a->price < b->price;  // Lower sell prices first
                  }
              });
    
    for (const auto* order : sorted_orders) {
        os << std::setw(12) << order->id
           << std::setw(12) << std::fixed << std::setprecision(2) << order->price
           << std::setw(12) << order->quantity
           << std::setw(8) << (order->is_buy() ? "BUY" : "SELL") << std::endl;
    }
    
    os << std::endl;
}

void OrderManager::print_snapshot_to_file(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    print_snapshot(file);
}

size_t OrderManager::load_from_csv(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    size_t loaded_count = 0;
    std::string line;
    
    // Skip header line if it exists
    std::getline(file, line);
    if (line.find("id") != std::string::npos || line.find("ID") != std::string::npos) {
        // This looks like a header, skip it
    } else {
        // Not a header, process this line
        auto order = parse_csv_line(line);
        if (order && add_order(std::move(*order))) {
            loaded_count++;
        }
    }
    
    // Process remaining lines
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        auto order = parse_csv_line(line);
        if (order && add_order(std::move(*order))) {
            loaded_count++;
        }
    }
    
    return loaded_count;
}

void OrderManager::print_stats(std::ostream& os) const {
    os << "\n=== ORDER MANAGER STATISTICS ===" << std::endl;
    os << "Active Orders: " << size() << std::endl;
    os << "Total Orders Added: " << total_orders_added_ << std::endl;
    os << "Total Orders Cancelled: " << total_orders_cancelled_ << std::endl;
    os << "Order Struct Size: " << sizeof(Order) << " bytes" << std::endl;
    os << "Memory Usage (estimate): " << (size() * sizeof(Order)) << " bytes" << std::endl;
    os << std::endl;
}

void OrderManager::clear() {
    orders_.clear();
    order_ptrs_.clear();
    snapshot_dirty_ = false;
    total_orders_added_ = 0;
    total_orders_cancelled_ = 0;
}

void OrderManager::rebuild_snapshot_cache() const {
    if (!snapshot_dirty_) return;
    
    order_ptrs_.clear();
    order_ptrs_.reserve(orders_.size());  // Pre-allocate to avoid reallocations
    
    for (const auto& [id, order] : orders_) {
        order_ptrs_.push_back(&order);
    }
    
    snapshot_dirty_ = false;
}

std::unique_ptr<Order> OrderManager::parse_csv_line(const std::string& line) const {
    std::istringstream iss(line);
    std::string token;
    
    try {
        // Parse order ID
        if (!std::getline(iss, token, ',')) return nullptr;
        uint64_t id = std::stoull(token);
        
        // Parse price
        if (!std::getline(iss, token, ',')) return nullptr;
        double price = std::stod(token);
        
        // Parse quantity
        if (!std::getline(iss, token, ',')) return nullptr;
        uint32_t quantity = std::stoul(token);
        
        // Parse side (0=buy, 1=sell)
        if (!std::getline(iss, token, ',')) return nullptr;
        uint32_t side = std::stoul(token);
        
        // Validate side
        if (side > 1) return nullptr;
        
        return std::make_unique<Order>(id, price, quantity, side);
        
    } catch (const std::exception& e) {
        // Invalid format, return nullptr
        return nullptr;
    }
} 