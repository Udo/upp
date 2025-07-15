#binary exe "../bin/forward-ref-simple-showcase"
#include <stdio.h>

int main() {
    
    int sum = add_numbers(15, 27);
    printf("   ✅ add_numbers(15, 27) = %d\n", sum);
    
    int product = multiply_nums(6, 8);  
    printf("   ✅ multiply_nums(6, 8) = %d\n", product);
    
    int power = calculate_power(2, 8);
    printf("   ✅ calculate_power(2, 8) = %d\n", power);

    // 🎯 TEST 2: Nested forward calls
    printf("\n📍 TEST 2: Nested Forward Function References\n");
    printf("   Functions calling other forward-declared functions\n\n");
    
    int complex_result = complex_math(4, 3);
    printf("   ✅ complex_math(4, 3) = %d\n", complex_result);
    
    int chained = chain_operations(5);
    printf("   ✅ chain_operations(5) = %d\n", chained);

    // 🎯 TEST 3: Recursive forward references
    printf("\n📍 TEST 3: Recursive Forward References\n");
    printf("   Self-referencing functions work perfectly\n\n");
    
    int fib = fibonacci(10);
    printf("   ✅ fibonacci(10) = %d\n", fib);
    
    int fact = factorial(6);
    printf("   ✅ factorial(6) = %d\n", fact);

    // 🎯 TEST 4: Mutually recursive functions
    printf("\n📍 TEST 4: Mutual Recursion with Forward References\n");
    printf("   Functions that call each other before definition\n\n");
    
    int even_result = is_even(8);
    int odd_result = is_odd(7);
    printf("   ✅ is_even(8) = %d (1=true, 0=false)\n", even_result);
    printf("   ✅ is_odd(7) = %d (1=true, 0=false)\n", odd_result);

    return 0;
}

// ===============================================================================
// FUNCTION DEFINITIONS - All defined AFTER main() but used FROM main()!
// ===============================================================================
// This would be impossible without forward reference support!

/* Basic math functions used in TEST 1 */
int add_numbers(int a, int b) {
    return a + b;
}

int multiply_nums(int x, int y) {
    return x * y;
}

int calculate_power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

/* Complex operations used in TEST 2 */
int complex_math(int a, int b) {
    // These also use forward references!
    int squared_a = square_it(a);        // Forward reference
    int cubed_b = cube_it(b);            // Forward reference
    return add_numbers(squared_a, cubed_b);  // Already defined above
}

int chain_operations(int start) {
    int doubled = double_it(start);      // Forward reference
    int incremented = increment_it(doubled); // Forward reference
    return square_it(incremented);       // Forward reference
}

/* Helper functions for complex math */
int square_it(int n) {
    return n * n;
}

int cube_it(int n) {
    return n * n * n;
}

int double_it(int n) {
    return n * 2;
}

int increment_it(int n) {
    return n + 1;
}

/* Recursive functions used in TEST 3 */
int fibonacci(int n) {
    if (n <= 1) return n;
    // Forward reference to itself - recursion!
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int factorial(int n) {
    if (n <= 1) return 1;
    // Forward reference to itself - recursion!
    return n * factorial(n - 1);
}

/* Mutually recursive functions used in TEST 4 */
int is_even(int n) {
    if (n == 0) return 1;
    return is_odd(n - 1);    // Forward reference to is_odd
}

int is_odd(int n) {
    if (n == 0) return 0;
    return is_even(n - 1);   // Forward reference to is_even
}
