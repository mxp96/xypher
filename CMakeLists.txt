cmake_minimum_required(VERSION 3.20)
project(xypher-lang VERSION 1.0.0 LANGUAGES C CXX)

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

find_package(ZLIB REQUIRED)
find_package(zstd)

# Find LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

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
    src/sema/ModuleRegistry.cpp
)

set(CODEGEN_SOURCES
    src/codegen/CodeGenerator.cpp
    src/codegen/LLVMBackend.cpp
    src/codegen/FFI.cpp
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

set(llvm_libs LLVM)

target_link_libraries(xypc ${llvm_libs} ZLIB::ZLIB)

if(TARGET zstd::libzstd_shared)
    target_link_libraries(xypc zstd::libzstd_shared)
elseif(TARGET zstd::libzstd_static)
    target_link_libraries(xypc zstd::libzstd_static)
endif()

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

