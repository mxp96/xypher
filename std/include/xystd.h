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
XYSTD_API double xy_abs_f64(double x);
XYSTD_API int xy_abs_i32(int x);

// Runtime initialization
XYSTD_API void xy_runtime_init(void);
XYSTD_API void xy_runtime_cleanup(void);

// Error handling
XYSTD_API void xy_panic(const char* message);
XYSTD_API void xy_assert(int condition, const char* message);

#ifdef __cplusplus
}
#endif

#endif // XYPHER_STD_H

