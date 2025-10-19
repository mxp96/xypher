#include "../include/xystd.h"
#include <string.h>

void xy_array_fill_i32(int* arr, unsigned long long size, int value) {
    if (!arr) return;
    for (unsigned long long i = 0; i < size; i++) {
        arr[i] = value;
    }
}

void xy_array_copy_i32(int* dest, const int* src, unsigned long long size) {
    if (!dest || !src) return;
    memcpy(dest, src, size * sizeof(int));
}

int xy_array_sum_i32(const int* arr, unsigned long long size) {
    if (!arr) return 0;
    int sum = 0;
    for (unsigned long long i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

int xy_array_min_i32(const int* arr, unsigned long long size) {
    if (!arr || size == 0) return 0;
    int min = arr[0];
    for (unsigned long long i = 1; i < size; i++) {
        if (arr[i] < min) min = arr[i];
    }
    return min;
}

int xy_array_max_i32(const int* arr, unsigned long long size) {
    if (!arr || size == 0) return 0;
    int max = arr[0];
    for (unsigned long long i = 1; i < size; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

