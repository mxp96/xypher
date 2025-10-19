# Xypher Standard Library (xystd)

Standard library untuk bahasa pemrograman Xypher, dikompilasi sebagai shared library.

## Library Names by Platform

- **Windows**: `xystd.dll` + `xystd.lib`
- **Linux**: `libxystd.so`
- **macOS**: `libxystd.dylib`

## Installation

Library otomatis di-build bersama compiler dan disimpan di:
```
build/bin/xystd.dll      (Windows)
build/bin/libxystd.so    (Linux)
build/bin/libxystd.dylib (macOS)
```

## API Functions

### Core I/O

```c
void xy_say_i32(int value);
void xy_say_i64(long long value);
void xy_say_f32(float value);
void xy_say_f64(double value);
void xy_say_str(const char* str);
void xy_say_bool(int value);
void xy_say_char(char c);
void xy_say_newline(void);
```

### Input

```c
int xy_grab_i32(void);
long long xy_grab_i64(void);
float xy_grab_f32(void);
double xy_grab_f64(void);
char* xy_grab_str(void);
```

### Debug Trace

```c
void xy_trace_i32(int value, const char* name);
void xy_trace_i64(long long value, const char* name);
void xy_trace_f32(float value, const char* name);
void xy_trace_f64(double value, const char* name);
void xy_trace_str(const char* str, const char* name);
void xy_trace_bool(int value, const char* name);
```

### Memory Management

```c
void* xy_alloc(unsigned long long size);
void xy_free(void* ptr);
void* xy_realloc(void* ptr, unsigned long long size);
```

### String Operations

```c
unsigned long long xy_strlen(const char* str);
char* xy_strcat(const char* s1, const char* s2);
int xy_strcmp(const char* s1, const char* s2);
char* xy_strcpy(char* dest, const char* src);
```

### Math Functions

```c
double xy_sqrt(double x);
double xy_pow(double base, double exp);
double xy_sin(double x);
double xy_cos(double x);
double xy_abs_f64(double x);
int xy_abs_i32(int x);
```

### Runtime

```c
void xy_runtime_init(void);
void xy_runtime_cleanup(void);
void xy_panic(const char* message);
void xy_assert(int condition, const char* message);
```

## Usage in C/C++

```c
#include <xystd.h>

int main() {
    xy_say_str("Hello from xystd!");
    xy_say_newline();
    
    int x = xy_grab_i32();
    xy_trace_i32(x, "user_input");
    
    return 0;
}
```

Compile:
```bash
clang program.c -L./bin -lxystd -o program
```

## Building

Standard library di-build otomatis bersama compiler:

```bash
./build.sh release
```

Atau manual:
```bash
cd build
cmake ..
cmake --build . --config Release
```

## Directory Structure

```
std/
├── include/
│   └── xystd.h        # Public API header
├── src/
│   ├── io.c          # I/O functions
│   ├── debug.c       # Debug/trace functions
│   ├── memory.c      # Memory management
│   ├── string.c      # String operations
│   ├── math.c        # Math functions
│   └── runtime.c     # Runtime initialization
├── CMakeLists.txt    # Build configuration
└── README.md         # This file
```

## Implementation Notes

- All functions are thread-safe where applicable
- Memory allocated by `xy_alloc` must be freed with `xy_free`
- String functions allocate new memory that must be freed
- Math functions follow IEEE 754 standards

## Version

1.0.0

## License

Apache 2.0

