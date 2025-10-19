#ifndef XYPHER_CONFIG_H
#define XYPHER_CONFIG_H

#define XYPHER_VERSION_MAJOR 1
#define XYPHER_VERSION_MINOR 0
#define XYPHER_VERSION_PATCH 0

#define XYPHER_VERSION_STRING "1.0.0"

// Language features
#define XYPHER_ENABLE_TYPE_INFERENCE 1
#define XYPHER_ENABLE_OWNERSHIP 1
#define XYPHER_ENABLE_ASYNC 1

// Optimization levels
enum class OptLevel {
    None = 0,
    O1,
    O2,
    O3,
    Os,
    Oz
};

#endif

