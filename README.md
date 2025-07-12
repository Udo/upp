# UPP - Ultimate++ Preprocessor

Fork of TCC (Tiny C Compiler) with UPP-specific build directives for Ultimate++ framework.

## Changes

### Build Directives

**#binary** - Specify output executable name:
```c
#binary exe "myapp"
```

**#param** - Set build parameters and compiler defines:
```c
#param DDEBUG "1"          // Define DEBUG=1
#param version "2.0"       // Store build parameter
#param g "true"            // Enable debug info (-g)
#param O2 "true"           // Optimization level (-O2)
```

**#compile** - Compile source files to object files:
```c
#compile obj "math_utils.c" "math_utils.o"
```
The compiler automatically checks if the source file is newer than the object file and only recompiles when necessary.

**#link** - Link object files into the final binary:
```c
#link obj "math_utils.o"
```
Object files are automatically linked into the final executable.

### General Features

- Parse source file directives before compilation
- The source file contains all the config/info necessary to produce the binary
- Multi-line string support

### Build the compiler

```bash
make
```

### Usage

```bash
./bin/upp myfile.c
```

### Complete Example

Here's a simple example demonstrating all build directives:

**calculator.c**:
```c
/* Simple calculator with #compile and #link directives */

#compile obj "math_utils.c" "math_utils.o"
#link obj "math_utils.o"
#binary exe "calculator"
#param g "true"            // Enable debug info

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
```

**math_utils.c**:
```c
/* Math utility functions */

#include <stdio.h>

int add(int a, int b) {
    printf("Adding %d + %d\n", a, b);
    return a + b;
}

int multiply(int a, int b) {
    printf("Multiplying %d * %d\n", a, b);
    return a * b;
}
```

Build and run:
```bash
./bin/upp calculator.c
./calculator
```

This will:
1. Compile `math_utils.c` to `math_utils.o`
2. Link `math_utils.o` into the final binary
3. Create executable named `calculator`
4. Enable debug info with `-g` flag

### Path Resolution

All file paths in directives are resolved relative to the source file containing the directive:

```c
#compile obj "src/utils.c" "build/utils.o"     // Relative to current file
#link obj "../lib/external.o"                  // Relative to current file
#binary exe "bin/myapp"                        // Relative to current file
```

This ensures that build directives work correctly regardless of the current working directory.

---
Based on TCC 0.9.27
