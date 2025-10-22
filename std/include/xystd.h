#ifndef XYPHER_STD_H
#define XYPHER_STD_H

#ifdef __cplusplus
extern "C" {
#endif

// Platform-specific export/import macros
#ifdef _WIN32
    #ifdef XYSTD_EXPORTS
        #define XYSTD_API __declspec(dllexport)
    #else
        #define XYSTD_API __declspec(dllimport)
    #endif
#else
    #define XYSTD_API __attribute__((visibility("default")))
#endif

// Standard library version
#define XYSTD_VERSION_MAJOR 1
#define XYSTD_VERSION_MINOR 0
#define XYSTD_VERSION_PATCH 0

// Core I/O functions
XYSTD_API void xy_say_i32(int value);
XYSTD_API void xy_say_i64(long long value);
XYSTD_API void xy_say_f32(float value);
XYSTD_API void xy_say_f64(double value);
XYSTD_API void xy_say_str(const char* str);
XYSTD_API void xy_say_bool(int value);
XYSTD_API void xy_say_char(char c);
XYSTD_API void xy_say_newline(void);

// Input functions
XYSTD_API int xy_grab_i32(void);
XYSTD_API long long xy_grab_i64(void);
XYSTD_API float xy_grab_f32(void);
XYSTD_API double xy_grab_f64(void);
XYSTD_API char* xy_grab_str(void);

// Debug trace functions
XYSTD_API void xy_trace_i32(int value, const char* name);
XYSTD_API void xy_trace_i64(long long value, const char* name);
XYSTD_API void xy_trace_f32(float value, const char* name);
XYSTD_API void xy_trace_f64(double value, const char* name);
XYSTD_API void xy_trace_str(const char* str, const char* name);
XYSTD_API void xy_trace_bool(int value, const char* name);

// Memory management
XYSTD_API void* xy_alloc(unsigned long long size);
XYSTD_API void xy_free(void* ptr);
XYSTD_API void* xy_realloc(void* ptr, unsigned long long size);

// String operations
XYSTD_API unsigned long long xy_strlen(const char* str);
XYSTD_API char* xy_strcat(const char* s1, const char* s2);
XYSTD_API int xy_strcmp(const char* s1, const char* s2);
XYSTD_API char* xy_strcpy(char* dest, const char* src);

// Math functions
XYSTD_API double xy_sqrt(double x);
XYSTD_API double xy_pow(double base, double exp);
XYSTD_API double xy_sin(double x);
XYSTD_API double xy_cos(double x);
XYSTD_API double xy_tan(double x);
XYSTD_API double xy_abs_f64(double x);
XYSTD_API int xy_abs_i32(int x);
XYSTD_API double xy_floor(double x);
XYSTD_API double xy_ceil(double x);
XYSTD_API double xy_round(double x);
XYSTD_API int xy_min_i32(int a, int b);
XYSTD_API int xy_max_i32(int a, int b);
XYSTD_API double xy_min_f64(double a, double b);
XYSTD_API double xy_max_f64(double a, double b);

// Array/Collection helpers
XYSTD_API void xy_array_fill_i32(int* arr, unsigned long long size, int value);
XYSTD_API void xy_array_copy_i32(int* dest, const int* src, unsigned long long size);
XYSTD_API int xy_array_sum_i32(const int* arr, unsigned long long size);
XYSTD_API int xy_array_min_i32(const int* arr, unsigned long long size);
XYSTD_API int xy_array_max_i32(const int* arr, unsigned long long size);

// File I/O
XYSTD_API void* xy_file_open(const char* filename, const char* mode);
XYSTD_API void xy_file_close(void* file);
XYSTD_API unsigned long long xy_file_read(void* file, void* buffer, unsigned long long size);
XYSTD_API unsigned long long xy_file_write(void* file, const void* buffer, unsigned long long size);
XYSTD_API char* xy_file_read_all(const char* filename);

// Runtime initialization
XYSTD_API void xy_runtime_init(void);
XYSTD_API void xy_runtime_cleanup(void);

// Time functions
XYSTD_API long long xy_time_ns(void);
XYSTD_API long long xy_time_us(void);
XYSTD_API long long xy_time_ms(void);
XYSTD_API long long xy_time_s(void);
XYSTD_API void xy_sleep_ms(int milliseconds);
XYSTD_API void xy_sleep_s(int seconds);

// Time helpers
XYSTD_API long long xy_time_diff_ms(long long start, long long end);
XYSTD_API double xy_time_to_s(long long ms);
XYSTD_API double xy_time_to_min(long long ms);
XYSTD_API double xy_time_to_h(long long ms);

// Error handling
XYSTD_API void xy_panic(const char* message);
XYSTD_API void xy_assert(int condition, const char* message);

#ifdef __cplusplus
}
#endif

#endif // XYPHER_STD_H

