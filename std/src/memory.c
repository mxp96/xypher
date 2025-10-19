#include "../include/xystd.h"
#include <stdlib.h>
#include <string.h>

void* xy_alloc(unsigned long long size) {
    return malloc((size_t)size);
}

void xy_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

void* xy_realloc(void* ptr, unsigned long long size) {
    return realloc(ptr, (size_t)size);
}

