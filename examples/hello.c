#binary exe "../bin/hello"
#include <stdio.h>

int main() {
    printf("Hello, World 
        from TCC/UPP!\n");
    #ifdef __BOUNDS_CHECKING_ON
    printf("Bounds checking is enabled.\n");
    #else
    printf("Bounds checking is NOT enabled.\n");
    #endif
    undefined_symbol_test();
    return 0;
}
