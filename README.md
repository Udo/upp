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


---
Based on TCC 0.9.27
