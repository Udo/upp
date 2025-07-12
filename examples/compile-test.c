/* Simple example demonstrating #compile and #link directives */

#compile obj "math_utils.c" "../build/math_utils.o"
#link obj "../build/math_utils.o"
#binary exe "../bin/compile-test"

#include <stdio.h>

// Function declarations from math_utils.c
int add(int a, int b);
int multiply(int a, int b);

int main() {
    int x = 10, y = 5;
    
    printf("Calculator Example\n");
    printf("==================\n");
    printf("%d + %d = %d\n", x, y, add(x, y));
    printf("%d * %d = %d\n", x, y, multiply(x, y));
    
    return 0;
}
