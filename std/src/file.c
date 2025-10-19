#include "../include/xystd.h"
#include <stdio.h>
#include <stdlib.h>

void* xy_file_open(const char* filename, const char* mode) {
    if (!filename || !mode) return NULL;
    return fopen(filename, mode);
}

void xy_file_close(void* file) {
    if (file) {
        fclose((FILE*)file);
    }
}

unsigned long long xy_file_read(void* file, void* buffer, unsigned long long size) {
    if (!file || !buffer) return 0;
    return (unsigned long long)fread(buffer, 1, (size_t)size, (FILE*)file);
}

unsigned long long xy_file_write(void* file, const void* buffer, unsigned long long size) {
    if (!file || !buffer) return 0;
    return (unsigned long long)fwrite(buffer, 1, (size_t)size, (FILE*)file);
}

char* xy_file_read_all(const char* filename) {
    if (!filename) return NULL;
    
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }
    
    fread(buffer, 1, size, f);
    buffer[size] = '\0';
    fclose(f);
    
    return buffer;
}

