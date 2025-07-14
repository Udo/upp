#binary exe "../bin/test-options"
#param O2 "true"
#param DDEBUG "1"
#param Wextra "true"
#include <stdio.h>

#ifdef DEBUG
#define DBG_PRINT(msg) printf("DEBUG: " msg "\n")
#else
#define DBG_PRINT(msg)
#endif

int main() {
    #ifdef __BOUNDS_CHECKING_ON
    DBG_PRINT("Test with optimization and debug flags");
    #endif
    printf("Hello from test with new command-line options!\n");
    return 0;
}
