cmake_minimum_required(VERSION 3.20)
project(xypher-lang VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type default to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX- /permissive-)
    # Suppress LLVM-related warnings in MSVC
    add_compile_options(
        /wd4244  # conversion from 'type1' to 'type2', possible loss of data
        /wd4245  # conversion from 'type1' to 'type2', signed/unsigned mismatch
        /wd4267  # conversion from 'size_t' to 'type', possible loss of data
        /wd4324  # structure was padded due to alignment specifier
        /wd4458  # declaration hides class member
        /wd4624  # destructor was implicitly defined as deleted
    )
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# DIA SDK support for Windows debugging symbols (following LLVM pattern)
option(LLVM_ENABLE_DIA_SDK "Use MSVC DIA SDK for debugging." ON)

if(MSVC AND LLVM_ENABLE_DIA_SDK)
    # Try to find MSVC_DIA_SDK_DIR
    if(NOT DEFINED MSVC_DIA_SDK_DIR)
        # Check environment variable first
        if(DEFINED ENV{MSVC_DIA_SDK_DIR})
            set(MSVC_DIA_SDK_DIR $ENV{MSVC_DIA_SDK_DIR})
        else()
            # Use VS installer to detect installation path
            if(DEFINED ENV{VSINSTALLDIR})
                set(MSVC_DIA_SDK_DIR "$ENV{VSINSTALLDIR}DIA SDK")
            elseif(DEFINED ENV{VCToolsInstallDir})
                get_filename_component(VS_ROOT "$ENV{VCToolsInstallDir}/../.." ABSOLUTE)
                set(MSVC_DIA_SDK_DIR "${VS_ROOT}/DIA SDK")
            endif()
        endif()
    endif()
    
    if(MSVC_DIA_SDK_DIR AND EXISTS "${MSVC_DIA_SDK_DIR}")
        include_directories(SYSTEM ${MSVC_DIA_SDK_DIR}/include)
        
        # Set library folder based on architecture (following LLVM pattern)
        set(LIBPDB_LINK_FOLDERS "${MSVC_DIA_SDK_DIR}/lib")
        
        if("$ENV{VSCMD_ARG_TGT_ARCH}" STREQUAL "arm64")
            set(LIBPDB_LINK_FOLDERS "${LIBPDB_LINK_FOLDERS}/arm64")
        elseif("$ENV{VSCMD_ARG_TGT_ARCH}" STREQUAL "arm")
            set(LIBPDB_LINK_FOLDERS "${LIBPDB_LINK_FOLDERS}/arm")
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(LIBPDB_LINK_FOLDERS "${LIBPDB_LINK_FOLDERS}/amd64")
        endif()
        
        link_directories(${LIBPDB_LINK_FOLDERS})
        
        message(STATUS "Found DIA SDK at: ${MSVC_DIA_SDK_DIR}")
        message(STATUS "  Include: ${MSVC_DIA_SDK_DIR}/include")
        message(STATUS "  Library: ${LIBPDB_LINK_FOLDERS}")
    else()
        message(WARNING "DIA SDK not found. Pass -DMSVC_DIA_SDK_DIR=<path> to cmake, or set LLVM_ENABLE_DIA_SDK=OFF")
        set(LLVM_ENABLE_DIA_SDK OFF)
    endif()
endif()

llvm_map_components_to_libnames(llvm_libs
    support core irreader executionengine
    interpreter mc mcjit bitwriter
    x86codegen x86asmparser x86desc x86info
    target passes orcjit nativecodegen
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Source files
set(LEXER_SOURCES
    src/lexer/Lexer.cpp
    src/lexer/Token.cpp
)

set(PARSER_SOURCES
    src/parser/Parser.cpp
)

set(AST_SOURCES
    src/ast/AST.cpp
    src/ast/ASTVisitor.cpp
    src/ast/ASTDumper.cpp
    src/ast/Module.cpp
    src/ast/Types.cpp
)

set(SEMA_SOURCES
    src/sema/SemanticAnalyzer.cpp
    src/sema/TypeChecker.cpp
    src/sema/SymbolTable.cpp
)

set(CODEGEN_SOURCES
    src/codegen/CodeGenerator.cpp
    src/codegen/LLVMBackend.cpp
)

set(FRONTEND_SOURCES
    src/frontend/Diagnostics.cpp
    src/frontend/SourceLocation.cpp
)

set(BACKEND_SOURCES
    src/backend/Optimizer.cpp
    src/backend/TargetMachine.cpp
    src/backend/JIT.cpp
)

set(RUNTIME_SOURCES
    src/runtime/Runtime.cpp
)

set(MAIN_SOURCE
    src/main.cpp
)

# Compiler executable
add_executable(xypc
    ${MAIN_SOURCE}
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${AST_SOURCES}
    ${SEMA_SOURCES}
    ${CODEGEN_SOURCES}
    ${FRONTEND_SOURCES}
    ${BACKEND_SOURCES}
    ${RUNTIME_SOURCES}
)

target_link_libraries(xypc ${llvm_libs})

# Copy compiler to bin directory after build
add_custom_command(TARGET xypc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:xypc> ${CMAKE_BINARY_DIR}/bin/
    COMMENT "Copying xypc compiler to bin directory"
)

# Install targets
install(TARGETS xypc DESTINATION bin)

# Testing
enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)

# Standard library (compile to bitcode)
add_subdirectory(std)

