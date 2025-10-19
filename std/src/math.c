#include "../include/xystd.h"
#include <math.h>

double xy_sqrt(double x) {
    return sqrt(x);
}

double xy_pow(double base, double exp) {
    return pow(base, exp);
}

double xy_sin(double x) {
    return sin(x);
}

double xy_cos(double x) {
    return cos(x);
}

double xy_tan(double x) {
    return tan(x);
}

double xy_abs_f64(double x) {
    return fabs(x);
}

int xy_abs_i32(int x) {
    return abs(x);
}

double xy_floor(double x) {
    return floor(x);
}

double xy_ceil(double x) {
    return ceil(x);
}

double xy_round(double x) {
    return round(x);
}

int xy_min_i32(int a, int b) {
    return a < b ? a : b;
}

int xy_max_i32(int a, int b) {
    return a > b ? a : b;
}

double xy_min_f64(double a, double b) {
    return a < b ? a : b;
}

double xy_max_f64(double a, double b) {
    return a > b ? a : b;
}

