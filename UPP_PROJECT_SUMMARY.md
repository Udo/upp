# UPP (Universal Prepass Parser) - Project Summary

## Overview
UPP is a TCC-based compiler with enhanced build features that support source file directives for controlling build output and parameters. This project successfully implements a comprehensive preprocessing system that extends TCC's capabilities.

## Implemented Features

### 1. String-String Hashmap (`src/upp/hash.h`)
- **Dynamic resizing** with 75% load factor management
- **Collision handling** using chaining for efficient key-value storage
- **Iterator support** for hashmap traversal
- **Memory-efficient design** using TCC's custom allocators
- **FNV-1a hash function** for good distribution
- **Thread-safe operations** with proper memory management
- **Header-only implementation** with static inline functions

### 2. Source File Directive Parser (`src/upp/prepass.h`)
- **Parses UPP directives** from source files before compilation
- **Supports `#param key "value"`** directives for build parameters
- **Supports `#binary exe "path"`** directives for output naming
- **Handles quoted and unquoted values** with proper C-style escaping
- **Integrates with compiler build process** seamlessly
- **Provides verbose output** for debugging and verification
- **Error handling** with detailed parse error reporting
- **No warnings during compilation** - UPP directives are fully recognized

### 3. Compiler Integration (`src/tcc.c`, `src/tcc.h`, `src/tcctok.h`, `src/tccpp.c`)
- **Prepass runs before main compilation** on the first source file
- **Parameters stored in compiler state** for access during build
- **Binary output path automatically set** from directives
- **Compatible with existing TCC features** and command-line options
- **Maintains C90 compatibility** with proper declarations
- **Memory management** integrated with TCC's allocator system
- **Native directive recognition** - `#param` and `#binary` are recognized tokens
- **Clean compilation** with no warnings about unknown directives

## Architecture

### Core Components
1. **Hashmap Engine** - Efficient key-value storage with dynamic resizing
2. **Directive Parser** - Robust parsing of UPP-specific directives
3. **Compiler Integration** - Seamless integration with TCC build process
4. **Memory Management** - TCC-compatible allocation and cleanup

### Data Flow
1. **Source File Analysis** - Prepass scans first source file for directives
2. **Parameter Extraction** - Directives parsed and stored in hashmap
3. **Compiler State Update** - Parameters and binary path set in TCC state
4. **Build Process** - Normal compilation proceeds with UPP parameters

## Usage Examples

### Basic Usage
```bash
# Compile with UPP directives (no warnings!)
upp -o myapp source.c

# Verbose mode to see prepass results
upp -vv source.c
```

### Source File Directives
```c
#param version "1.0.0"
#param author "Developer Name"
#param optimization "fast"
#binary exe "my_application"

#include <stdio.h>
int main() { return 0; }
```

### Clean Compilation Output
```
UPP Prepass Results:
  Directives processed: 4
  Parse errors: 0
  Binary path: my_application
  Parameters:
    version = 1.0.0
    author = Developer Name
    optimization = fast
```
*Note: No warnings about unknown preprocessing directives!*

## Technical Details

### Hash Function
- Uses FNV-1a algorithm for good distribution
- 32-bit hash with proper collision handling
- Optimized for string keys

### Memory Management
- Compatible with TCC's `tcc_malloc`/`tcc_free` system
- Automatic cleanup on compiler shutdown
- No memory leaks in normal operation

### Parser Features
- Handles C-style string escaping
- Supports both quoted and unquoted values
- Robust error handling and reporting
- Line-based parsing with proper tokenization

## Testing

### Test Suite
- **Hashmap tests** - Comprehensive testing of all hashmap operations
- **Prepass tests** - Directive parsing and integration tests
- **Integration tests** - End-to-end compilation and execution tests
- **Edge case tests** - Various parameter formats and error conditions

### Test Results
- All core functionality working correctly
- Proper handling of various directive formats
- Correct binary naming from directives
- Parameter storage and retrieval verified

## Build System

### Compilation
```bash
# Clean build
make clean

# Build UPP compiler
make

# Runs successfully with GCC and TCC
```

### Compatibility
- **C90 compliant** (with minor warnings)
- **Linux x86_64** primary target
- **TCC compatible** memory management
- **Header-only design** for easy integration

## Future Extensions

### Potential Enhancements
1. **Runtime parameter access** - Allow compiled programs to access UPP parameters
2. **Additional directive types** - Support for more build control directives
3. **Configuration files** - Support for external UPP configuration
4. **IDE integration** - Better integration with development environments
5. **Cross-compilation** - Enhanced support for different target platforms

### Advanced Features
1. **Conditional compilation** - Directives that control compilation flow
2. **Build dependencies** - Automatic dependency tracking from directives
3. **Resource embedding** - Embed files directly into executables
4. **Optimization hints** - Compiler optimization guidance from directives

## Conclusion

The UPP project successfully implements a comprehensive preprocessing system that enhances TCC with powerful build-time directives. The implementation is robust, well-tested, and integrates seamlessly with existing TCC functionality. All core features are working correctly and the system is ready for production use.

### Key Achievements
- ✅ Robust string-string hashmap implementation
- ✅ Complete directive parser with error handling
- ✅ Seamless compiler integration
- ✅ Comprehensive test coverage
- ✅ Memory-safe design with proper cleanup
- ✅ C90 compatibility maintained
- ✅ Production-ready build system

The UPP compiler demonstrates that TCC can be successfully extended with sophisticated preprocessing capabilities while maintaining its simplicity and performance characteristics.
