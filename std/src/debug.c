#include "../include/xystd.h"
#include <stdio.h>

void xy_trace_i32(int value, const char* name) {
    printf("[TRACE] %s: i32 = %d\n", name ? name : "<expr>", value);
}

void xy_trace_i64(long long value, const char* name) {
    printf("[TRACE] %s: i64 = %lld\n", name ? name : "<expr>", value);
}

void xy_trace_f32(float value, const char* name) {
    printf("[TRACE] %s: f32 = %f\n", name ? name : "<expr>", value);
}

void xy_trace_f64(double value, const char* name) {
    printf("[TRACE] %s: f64 = %f\n", name ? name : "<expr>", value);
}

void xy_trace_str(const char* str, const char* name) {
    printf("[TRACE] %s: str = \"%s\"\n", name ? name : "<expr>", str ? str : "(null)");
}

void xy_trace_bool(int value, const char* name) {
    printf("[TRACE] %s: bool = %s\n", name ? name : "<expr>", value ? "true" : "false");
}

