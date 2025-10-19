#include "../include/xystd.h"
#include <stdio.h>
#include <stdlib.h>

void xy_say_i32(int value) {
    printf("%d", value);
}

void xy_say_i64(long long value) {
    printf("%lld", value);
}

void xy_say_f32(float value) {
    printf("%f", value);
}

void xy_say_f64(double value) {
    printf("%f", value);
}

void xy_say_str(const char* str) {
    if (str) {
        printf("%s", str);
    }
}

void xy_say_bool(int value) {
    printf("%s", value ? "true" : "false");
}

void xy_say_char(char c) {
    printf("%c", c);
}

void xy_say_newline(void) {
    printf("\n");
}

int xy_grab_i32(void) {
    int value;
    if (scanf("%d", &value) == 1) {
        return value;
    }
    return 0;
}

long long xy_grab_i64(void) {
    long long value;
    if (scanf("%lld", &value) == 1) {
        return value;
    }
    return 0;
}

float xy_grab_f32(void) {
    float value;
    if (scanf("%f", &value) == 1) {
        return value;
    }
    return 0.0f;
}

double xy_grab_f64(void) {
    double value;
    if (scanf("%lf", &value) == 1) {
        return value;
    }
    return 0.0;
}

char* xy_grab_str(void) {
    static char buffer[1024];
    if (scanf("%1023s", buffer) == 1) {
        return buffer;
    }
    return NULL;
}

