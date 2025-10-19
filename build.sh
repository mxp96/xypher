#!/bin/bash
set -e

BUILD_TYPE="Release"
RUN_TESTS=false

case "$1" in
    debug) BUILD_TYPE="Debug" ;;
    release) BUILD_TYPE="Release" ;;
    test) RUN_TESTS=true ;;
    clean) rm -rf build && echo "Clean complete" && exit 0 ;;
    "") ;;
    *) echo "Usage: $0 [debug|release|test|clean]" && exit 1 ;;
esac

GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}=== Xypher Build ===${NC}"

command -v cmake >/dev/null || { echo "Error: CMake not found"; exit 1; }

mkdir -p build
cd build

echo "Configuring ($BUILD_TYPE)..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE || exit 1

echo "Building compiler and stdlib..."
NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j$NPROC || exit 1

echo -e "\n${GREEN}=== Build Complete ===${NC}"
echo "Output: build/bin/"
echo "  - xypc"
echo "  - libxystd.so"

if [ "$RUN_TESTS" = true ]; then
    echo -e "\n${GREEN}=== Running Tests ===${NC}"
    cd ..
    mkdir -p output
    for f in examples/*.xyp; do
        name=$(basename "$f" .xyp)
        echo -n "Testing $name... "
        if build/bin/xypc "$f" -o "output/$name" &>/dev/null; then
            if "output/$name" &>/dev/null; then
                echo "[OK]"
            fi
        fi
    done
fi

