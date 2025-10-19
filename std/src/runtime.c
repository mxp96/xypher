#include "../include/xystd.h"
#include <stdio.h>
#include <stdlib.h>

static int runtime_initialized = 0;

void xy_runtime_init(void) {
    if (!runtime_initialized) {
        runtime_initialized = 1;
    }
}

void xy_runtime_cleanup(void) {
    if (runtime_initialized) {
        runtime_initialized = 0;
    }
}

void xy_panic(const char* message) {
    fprintf(stderr, "\n[PANIC] %s\n", message ? message : "Unknown error");
    exit(1);
}

void xy_assert(int condition, const char* message) {
    if (!condition) {
        fprintf(stderr, "\n[ASSERTION FAILED] %s\n", message ? message : "Assertion failed");
        exit(1);
    }
}

