# Xypher Standard Library

Core runtime library for Xypher programs.

## Library Files

- **Windows**: `xystd.dll` + `xystd.lib`
- **Linux**: `libxystd.so`
- **macOS**: `libxystd.dylib`

Location: `build/bin/`

## Modules

### I/O (8 functions)
Output: `xy_say_i32`, `xy_say_f64`, `xy_say_str`, `xy_say_newline`  
Input: `xy_grab_i32`, `xy_grab_f64`, `xy_grab_str`

### Debug (6 functions)
Trace: `xy_trace_i32`, `xy_trace_f64`, `xy_trace_str`, etc.

### Memory (3 functions)
`xy_alloc`, `xy_free`, `xy_realloc`

### String (4 functions)
`xy_strlen`, `xy_strcat`, `xy_strcmp`, `xy_strcpy`

### Math (14 functions)
Basic: `xy_sqrt`, `xy_pow`, `xy_sin`, `xy_cos`, `xy_tan`  
Rounding: `xy_floor`, `xy_ceil`, `xy_round`  
Comparison: `xy_min_i32`, `xy_max_i32`, `xy_min_f64`, `xy_max_f64`  
Utils: `xy_abs_i32`, `xy_abs_f64`

### Arrays (5 functions)
`xy_array_fill_i32`, `xy_array_copy_i32`, `xy_array_sum_i32`  
`xy_array_min_i32`, `xy_array_max_i32`

### Hashmap (10 functions)
`xy_hashmap_create`, `xy_hashmap_destroy`, `xy_hashmap_insert`, `xy_hashmap_get`  
`xy_hashmap_remove`, `xy_hashmap_contains`, `xy_hashmap_size`, `xy_hashmap_clear`  
`xy_hashmap_keys`, `xy_hashmap_free_keys`

### File I/O (5 functions)
`xy_file_open`, `xy_file_close`, `xy_file_read`, `xy_file_write`, `xy_file_read_all`

### Runtime
`xy_runtime_init`, `xy_runtime_cleanup`, `xy_panic`, `xy_assert`

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
