#!/bin/bash
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}======================================${NC}"
echo -e "${BLUE}  Xypher Testing Deployment (Linux)${NC}"
echo -e "${BLUE}======================================${NC}"
echo ""

if [ ! -d "build" ]; then
    echo -e "${RED}Error: build directory not found!${NC}"
    echo -e "${YELLOW}Please run ./build.sh first${NC}"
    exit 1
fi

if [ ! -f "build/bin/xypc" ]; then
    echo -e "${RED}Error: xypc not found in build/bin/${NC}"
    echo -e "${YELLOW}Please build the project first: ./build.sh${NC}"
    exit 1
fi

if [ ! -f "build/bin/libxystd.so" ]; then
    echo -e "${RED}Error: libxystd.so not found in build/bin/${NC}"
    echo -e "${YELLOW}Please build the project first: ./build.sh${NC}"
    exit 1
fi

echo -e "${YELLOW}Creating bin directory...${NC}"
mkdir -p bin

echo -e "${YELLOW}Copying xypc compiler...${NC}"
cp -v build/bin/xypc bin/

echo -e "${YELLOW}Copying libxystd.so standard library...${NC}"
cp -v build/bin/libxystd.so bin/

echo -e "${YELLOW}Setting executable permissions...${NC}"
chmod +x bin/xypc

echo ""
echo -e "${GREEN}======================================${NC}"
echo -e "${GREEN}  Deployment Successful!${NC}"
echo -e "${GREEN}======================================${NC}"
echo ""

echo -e "${BLUE}Files copied:${NC}"
ls -lh bin/xypc
ls -lh bin/libxystd.so

echo ""
echo -e "${BLUE}File sizes:${NC}"
du -h bin/xypc
du -h bin/libxystd.so

echo ""
echo -e "${GREEN}Compiler version:${NC}"
./bin/xypc --version

echo ""
echo -e "${YELLOW}Testing deployment:${NC}"
echo -e "  Compiler: ${GREEN}./bin/xypc${NC}"
echo -e "  Library:  ${GREEN}./bin/libxystd.so${NC}"
echo ""

echo -e "${BLUE}Usage examples:${NC}"
echo -e "  ${GREEN}./bin/xypc examples/hello.xyp -o hello${NC}"
echo -e "  ${GREEN}./hello${NC}"
echo ""

echo -e "${BLUE}Add to PATH (optional):${NC}"
echo -e "  ${YELLOW}export PATH=\"\$PATH:$(pwd)/bin\"${NC}"
echo ""

echo -e "${GREEN}Ready for testing!${NC}"

