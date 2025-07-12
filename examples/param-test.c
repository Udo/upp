/* Comprehensive UPP parameter test */

#param version "2.0"
#param DDEBUG "1"
#param DMEMORY_SIZE "1024"
#param DNAME "\"MyAppTest\""
#param Iinclude_dir
#param Llib_dir
#param lmath
#binary exe "../bin/param-test"

#include <stdio.h>

int main() {
    printf("=== Comprehensive UPP Parameter Test ===\n");
    printf("Memory Size: %d\n", MEMORY_SIZE);
    printf("App Name: %s\n", NAME);
    
#ifdef DEBUG
    printf("DEBUG is enabled with value: %d\n", DEBUG);
#else
    printf("DEBUG is not enabled\n");
#endif
    
    return 0;
}
