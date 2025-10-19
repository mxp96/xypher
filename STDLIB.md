# Xypher Standard Library

Runtime library providing core functionality for Xypher programs.

## Library Files

- **Windows**: `xystd.dll` + `xystd.lib`
- **Linux**: `libxystd.so`
- **macOS**: `libxystd.dylib`

Location: `build/bin/`

## Modules

### I/O

```c
void xy_say_i32(int value);
void xy_say_f64(double value);
void xy_say_str(const char* str);
void xy_say_newline(void);

int xy_grab_i32(void);
double xy_grab_f64(void);
char* xy_grab_str(void);
```

### Debug

```c
void xy_trace_i32(int value, const char* name);
void xy_trace_f64(double value, const char* name);
void xy_trace_str(const char* str, const char* name);
```

### Memory

```c
void* xy_alloc(unsigned long long size);
void xy_free(void* ptr);
void* xy_realloc(void* ptr, unsigned long long size);
```

### String

```c
unsigned long long xy_strlen(const char* str);
char* xy_strcat(const char* s1, const char* s2);
int xy_strcmp(const char* s1, const char* s2);
```

### Math

```c
double xy_sqrt(double x);
double xy_pow(double base, double exp);
double xy_sin(double x);
double xy_cos(double x);
```

### Runtime

```c
void xy_runtime_init(void);
void xy_panic(const char* message);
void xy_assert(int condition, const char* message);
```

## Usage from Xypher

```xypher
say("Hello");           // Uses xy_say_str
let x = grab();         // Uses xy_grab_i32
trace(value);           // Uses xy_trace_i32
```

## Usage from C

```c
#include <xystd.h>

int main() {
    xy_say_str("Hello from C!");
    xy_say_newline();
    return 0;
}
```

Compile:
```bash
clang program.c -Lbuild/bin -lxystd -o program
```

## Linking

Compiler auto-links with xystd. No manual steps needed.

## Distribution

When distributing programs, include `xystd.dll/.so/.dylib` in the same folder or system library path.

## Building Only StdLib

```bash
cd build
cmake --build . --target xystd
```

Output: `build/bin/xystd.dll`

## API Reference

See `std/include/xystd.h` for complete API.

Full documentation in header comments.
