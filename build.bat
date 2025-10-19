@echo off
setlocal enabledelayedexpansion

set BUILD_TYPE=Release
set RUN_TESTS=0

if "%1"=="debug" set BUILD_TYPE=Debug
if "%1"=="release" set BUILD_TYPE=Release
if "%1"=="test" set RUN_TESTS=1
if "%1"=="clean" (
    rmdir /s /q build 2>nul
    echo Clean complete
    exit /b 0
)

echo === Xypher Build ===

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: CMake not found
    exit /b 1
)

if not exist build mkdir build

echo Configuring (%BUILD_TYPE%)...
cmake -S . -B build -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% neq 0 exit /b 1

echo Building compiler and stdlib...
cmake --build build --config %BUILD_TYPE%
if %ERRORLEVEL% neq 0 exit /b 1

echo.
echo === Build Complete ===
echo Output: build\bin\
echo   - xypc.exe (Compiler with JIT)
echo   - xystd.dll (49 functions)

if %RUN_TESTS%==1 (
    echo.
    echo === Running Tests ===
    call :run_tests
)

exit /b 0

:run_tests
set COMPILER=build\bin\xypc.exe
set OUTPUT=output
if not exist %OUTPUT% mkdir %OUTPUT%

for %%f in (examples\*.xyp) do (
    echo Testing %%~nf...
    %COMPILER% %%f -o %OUTPUT%\%%~nf
    if !ERRORLEVEL! equ 0 (
        %OUTPUT%\%%~nf.exe > nul
        if !ERRORLEVEL! equ 0 echo   [OK] %%~nf
    )
)
exit /b 0

