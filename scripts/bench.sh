#!/bin/bash

# Benchmark Script for Limit Order Manager
# Runs comprehensive performance tests and generates reports

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="."
EXECUTABLE="limit_order_manager"
DATA_DIR="data"
RESULTS_DIR="results"
LOG_FILE="benchmark.log"

# Create results directory
mkdir -p "$RESULTS_DIR"

echo -e "${BLUE}=== Limit Order Manager Benchmark Suite ===${NC}"
echo "Starting comprehensive performance tests..."
echo "Results will be saved to: $RESULTS_DIR"
echo "Log file: $LOG_FILE"
echo

# Function to log messages
log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') - $1" | tee -a "$LOG_FILE"
}

# Function to run a benchmark
run_benchmark() {
    local test_name="$1"
    local order_count="$2"
    local output_file="$RESULTS_DIR/${test_name}_${order_count}.txt"
    
    log "Running $test_name with $order_count orders..."
    
    # Run the benchmark and capture output
    if timeout 300 "$BUILD_DIR/$EXECUTABLE" benchmark "$order_count" > "$output_file" 2>&1; then
        echo -e "${GREEN}✓${NC} $test_name completed successfully"
        log "$test_name completed successfully"
    else
        echo -e "${RED}✗${NC} $test_name failed or timed out"
        log "$test_name failed or timed out"
        return 1
    fi
}

# Function to check if executable exists
check_executable() {
    if [[ ! -f "$BUILD_DIR/$EXECUTABLE" ]]; then
        echo -e "${RED}Error: Executable not found at $BUILD_DIR/$EXECUTABLE${NC}"
        echo "Please build the project first: make release"
        exit 1
    fi
}

# Function to generate test data
generate_test_data() {
    log "Generating test data..."
    
    # Generate different sized datasets
    python3 scripts/gen_orders.py 1000 -o "$DATA_DIR/test_1k.txt"
    python3 scripts/gen_orders.py 10000 -o "$DATA_DIR/test_10k.txt"
    python3 scripts/gen_orders.py 100000 -o "$DATA_DIR/test_100k.txt"
    
    echo -e "${GREEN}✓${NC} Test data generated"
}

# Function to run memory leak detection
run_memcheck() {
    log "Running memory leak detection..."
    
    if command -v valgrind >/dev/null 2>&1; then
        valgrind --leak-check=full --show-leak-kinds=all \
                 --log-file="$RESULTS_DIR/memcheck.log" \
                 "$BUILD_DIR/$EXECUTABLE" benchmark 1000 >/dev/null 2>&1
        
        if grep -q "definitely lost: 0 bytes" "$RESULTS_DIR/memcheck.log"; then
            echo -e "${GREEN}✓${NC} No memory leaks detected"
            log "No memory leaks detected"
        else
            echo -e "${RED}✗${NC} Memory leaks detected - check $RESULTS_DIR/memcheck.log"
            log "Memory leaks detected"
        fi
    else
        echo -e "${YELLOW}⚠${NC} Valgrind not available - skipping memory check"
        log "Valgrind not available - skipping memory check"
    fi
}

# Function to run basic functionality tests
run_functional_tests() {
    log "Running functional tests..."
    
    # Test 1: Load sample data
    if "$BUILD_DIR/$EXECUTABLE" load "$DATA_DIR/ticks.txt" > "$RESULTS_DIR/functional_test1.txt" 2>&1; then
        echo -e "${GREEN}✓${NC} Sample data loading test passed"
    else
        echo -e "${RED}✗${NC} Sample data loading test failed"
        return 1
    fi
    
    # Test 2: Generate orders
    if "$BUILD_DIR/$EXECUTABLE" generate 100 > "$RESULTS_DIR/functional_test2.txt" 2>&1; then
        echo -e "${GREEN}✓${NC} Order generation test passed"
    else
        echo -e "${RED}✗${NC} Order generation test failed"
        return 1
    fi
    
    # Test 3: Interactive mode (basic test)
    echo "quit" | timeout 10 "$BUILD_DIR/$EXECUTABLE" interactive > "$RESULTS_DIR/functional_test3.txt" 2>&1 || true
    
    log "Functional tests completed"
}

# Function to generate performance report
generate_report() {
    log "Generating performance report..."
    
    local report_file="$RESULTS_DIR/performance_report.txt"
    
    cat > "$report_file" << EOF
Limit Order Manager - Performance Report
Generated: $(date)
========================================

Test Results Summary:
EOF
    
    # Collect timing information from benchmark results
    for result_file in "$RESULTS_DIR"/benchmark_*.txt; do
        if [[ -f "$result_file" ]]; then
            echo "" >> "$report_file"
            echo "File: $(basename "$result_file")" >> "$report_file"
            echo "----------------------------------------" >> "$report_file"
            grep -E "(took|microseconds|orders)" "$result_file" >> "$report_file" || true
        fi
    done
    
    echo -e "${GREEN}✓${NC} Performance report generated: $report_file"
    log "Performance report generated"
}

# Main execution
main() {
    echo "Starting benchmark suite at $(date)"
    log "Starting benchmark suite"
    
    # Check prerequisites
    check_executable
    
    # Generate test data
    generate_test_data
    
    # Run functional tests
    run_functional_tests
    
    # Run memory leak detection
    run_memcheck
    
    # Run performance benchmarks
    echo -e "${BLUE}Running performance benchmarks...${NC}"
    
    run_benchmark "benchmark" 1000
    run_benchmark "benchmark" 10000
    run_benchmark "benchmark" 100000
    
    # Generate final report
    generate_report
    
    echo
    echo -e "${GREEN}=== Benchmark Suite Completed ===${NC}"
    echo "Results saved to: $RESULTS_DIR"
    echo "Log file: $LOG_FILE"
    log "Benchmark suite completed successfully"
}

# Run main function
main "$@" 