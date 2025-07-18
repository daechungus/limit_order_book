#!/usr/bin/env python3
"""
Order Generator Script
Generates large CSV files with random order data for performance testing.
"""

import csv
import random
import argparse
import sys
from typing import List, Tuple

def generate_orders(count: int, price_range: Tuple[float, float] = (100.0, 200.0),
                   qty_range: Tuple[int, int] = (1, 1000)) -> List[Tuple[int, float, int, int]]:
    """
    Generate random orders for testing.
    
    Args:
        count: Number of orders to generate
        price_range: (min_price, max_price) tuple
        qty_range: (min_qty, max_qty) tuple
    
    Returns:
        List of (id, price, quantity, side) tuples
    """
    orders = []
    
    for i in range(1, count + 1):
        order_id = i
        price = round(random.uniform(price_range[0], price_range[1]), 2)
        quantity = random.randint(qty_range[0], qty_range[1])
        side = random.randint(0, 1)  # 0 = buy, 1 = sell
        
        orders.append((order_id, price, quantity, side))
    
    return orders

def write_csv(filename: str, orders: List[Tuple[int, float, int, int]], 
              include_header: bool = True):
    """
    Write orders to CSV file.
    
    Args:
        filename: Output file path
        orders: List of order tuples
        include_header: Whether to include CSV header
    """
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        if include_header:
            writer.writerow(['id', 'price', 'quantity', 'side'])
        
        for order in orders:
            writer.writerow(order)

def main():
    parser = argparse.ArgumentParser(description='Generate random order data for testing')
    parser.add_argument('count', type=int, help='Number of orders to generate')
    parser.add_argument('-o', '--output', default='data/generated_orders.txt',
                       help='Output file path (default: data/generated_orders.txt)')
    parser.add_argument('--min-price', type=float, default=100.0,
                       help='Minimum price (default: 100.0)')
    parser.add_argument('--max-price', type=float, default=200.0,
                       help='Maximum price (default: 200.0)')
    parser.add_argument('--min-qty', type=int, default=1,
                       help='Minimum quantity (default: 1)')
    parser.add_argument('--max-qty', type=int, default=1000,
                       help='Maximum quantity (default: 1000)')
    parser.add_argument('--no-header', action='store_true',
                       help='Skip CSV header row')
    
    args = parser.parse_args()
    
    if args.count <= 0:
        print("Error: Count must be positive", file=sys.stderr)
        sys.exit(1)
    
    if args.min_price >= args.max_price:
        print("Error: min_price must be less than max_price", file=sys.stderr)
        sys.exit(1)
    
    if args.min_qty >= args.max_qty:
        print("Error: min_qty must be less than max_qty", file=sys.stderr)
        sys.exit(1)
    
    print(f"Generating {args.count} orders...")
    
    # Set random seed for reproducible results
    random.seed(42)
    
    # Generate orders
    orders = generate_orders(
        args.count,
        price_range=(args.min_price, args.max_price),
        qty_range=(args.min_qty, args.max_qty)
    )
    
    # Write to file
    write_csv(args.output, orders, include_header=not args.no_header)
    
    print(f"Generated {len(orders)} orders in {args.output}")
    
    # Print some statistics
    prices = [order[1] for order in orders]
    quantities = [order[2] for order in orders]
    sides = [order[3] for order in orders]
    
    print(f"Price range: {min(prices):.2f} - {max(prices):.2f}")
    print(f"Quantity range: {min(quantities)} - {max(quantities)}")
    print(f"Buy orders: {sides.count(0)}")
    print(f"Sell orders: {sides.count(1)}")

if __name__ == '__main__':
    main() 