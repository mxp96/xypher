#include "../include/xystd.h"
#include <string.h>
#include <stdlib.h>

unsigned long long xy_strlen(const char* str) {
    if (!str) return 0;
    return (unsigned long long)strlen(str);
}

char* xy_strcat(const char* s1, const char* s2) {
    if (!s1 || !s2) return NULL;
    
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char* result = (char*)malloc(len1 + len2 + 1);
    
    if (result) {
        strcpy(result, s1);
        strcat(result, s2);
    }
    
    return result;
}

int xy_strcmp(const char* s1, const char* s2) {
    if (!s1 || !s2) return -1;
    return strcmp(s1, s2);
}

char* xy_strcpy(char* dest, const char* src) {
    if (!dest || !src) return NULL;
    return strcpy(dest, src);
}

