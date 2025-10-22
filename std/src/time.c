#include "../include/xystd.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
    #include <time.h>
#endif

long long xy_time_ns(void) {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000000LL) / frequency.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)(ts.tv_sec) * 1000000000LL + (long long)(ts.tv_nsec);
#endif
}

long long xy_time_us(void) {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000LL) / frequency.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000000LL + (long long)(tv.tv_usec);
#endif
}

long long xy_time_ms(void) {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000LL) / frequency.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec) * 1000LL + (long long)(tv.tv_usec) / 1000LL;
#endif
}

long long xy_time_s(void) {
#ifdef _WIN32
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return counter.QuadPart / frequency.QuadPart;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long long)(tv.tv_sec);
#endif
}

void xy_sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void xy_sleep_s(int seconds) {
#ifdef _WIN32
    Sleep(seconds * 1000);
#else
    sleep(seconds);
#endif
}

long long xy_time_diff_ms(long long start, long long end) {
    return end - start;
}

double xy_time_to_s(long long ms) {
    return (double)ms / 1000.0;
}

double xy_time_to_min(long long ms) {
    return (double)ms / 60000.0;
}

double xy_time_to_h(long long ms) {
    return (double)ms / 3600000.0;
}



