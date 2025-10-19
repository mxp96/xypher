# Deployment Guide

How to package and distribute Xypher compiler and programs.

## Compiler Distribution

### Build Release

```bash
./build.bat      # Windows
./build.sh       # Linux/macOS
```

### Package Structure

```
xypher/
└── bin/
    ├── xypc(.exe)       # Compiler
    └── xystd.dll/.so    # Standard library
```

### Installation

**Portable**: Copy `bin/` folder anywhere. Compiler auto-detects library.

**System-wide** (Linux/macOS):
```bash
sudo cmake --install build --prefix /usr/local
```

**PATH** (Windows):
```bash
set PATH=%PATH%;C:\path\to\xypher\bin
```

## Program Distribution

Compiled programs need `xystd` library at runtime.

### Option 1: Bundle Library

```
myprogram/
├── myprogram.exe
└── xystd.dll        # Copy from build/bin/
```

### Option 2: System Library

**Windows**: Add `xystd.dll` to PATH  
**Linux**: `sudo cp libxystd.so /usr/local/lib && sudo ldconfig`  
**macOS**: `sudo cp libxystd.dylib /usr/local/lib`

## Auto-Detection

Compiler finds `xystd` library by checking:
1. Same directory as compiler
2. `../bin/` from compiler
3. `../../bin/` (for VS builds)
4. `lib/` subdirectory

## Creating Packages

### Windows Installer (NSIS)

```nsis
OutFile "xypher-setup.exe"
InstallDir "$PROGRAMFILES\Xypher"

Section
    SetOutPath $INSTDIR\bin
    File "build\bin\xypc.exe"
    File "build\bin\xystd.dll"
    EnVar::AddValue "PATH" "$INSTDIR\bin"
SectionEnd
```

### Linux Package (.deb)

```bash
mkdir -p pkg/usr/local/bin
cp build/bin/{xypc,libxystd.so} pkg/usr/local/bin/
dpkg-deb --build pkg xypher-1.0.deb
```

### Portable Archive

**Windows**:
```bash
xcopy /E build\bin xypher-portable\
zip -r xypher-win64.zip xypher-portable
```

**Linux/macOS**:
```bash
cp -r build/bin xypher-portable/
tar czf xypher-linux64.tar.gz xypher-portable/
```

## Docker

```dockerfile
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y llvm-14-dev clang cmake build-essential
COPY . /xypher
WORKDIR /xypher
RUN ./build.sh
ENV PATH="/xypher/build/bin:$PATH"
```

## Testing Before Release

```bash
.\build.bat test
```

## Release Checklist

- [ ] Update version in `XypherConfig.h`
- [ ] Update `CHANGELOG.md`
- [ ] Build on all platforms
- [ ] Test all examples
- [ ] Create packages
- [ ] Tag release
- [ ] Upload binaries

## Troubleshooting

**Library not found at runtime?**  
Copy `xystd.dll` next to your program, or add to PATH/LD_LIBRARY_PATH.

**Compiler can't find library?**  
Make sure `xystd.dll` is in same directory as `xypc.exe`.

See [README.md](README.md) for more help.
