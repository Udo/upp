# UPP - Udo's Preprocessor Hacks for C

Fork of TCC (Tiny C Compiler).

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
- **Automatic typedefs**: Named structs, enums, and unions automatically create corresponding typedefs

#### Automatic Typedefs

When you define a named struct, enum, or union, UPP automatically creates a typedef with the same name, allowing you to use the type without the `struct`/`enum`/`union` keyword:

```c
struct Point { int x, y; };        // Also creates: typedef struct Point Point;
enum Color { RED, GREEN, BLUE };   // Also creates: typedef enum Color Color;  
union Value { int i; float f; };   // Also creates: typedef union Value Value;

// Now you can use them directly:
Point p = {10, 20};     // Instead of: struct Point p = {10, 20};
Color c = RED;          // Instead of: enum Color c = RED;
Value v;                // Instead of: union Value v;
```

See `examples/auto-typedef.c` for a complete example.

### Build the compiler

```bash
make
```

### Usage

```bash
./bin/upp myfile.c
```


---
Based on TCC 0.9.27
