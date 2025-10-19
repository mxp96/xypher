# Xypher Standard Library
# Compiled as shared library (xystd.dll/.so/.dylib)

project(xystd VERSION 1.0.0 LANGUAGES C)

# Source files
set(XYSTD_SOURCES
    src/io.c
    src/debug.c
    src/memory.c
    src/string.c
    src/math.c
    src/runtime.c
)

# Build shared library
add_library(xystd SHARED ${XYSTD_SOURCES})

# Set library properties
set_target_properties(xystd PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    PUBLIC_HEADER include/xystd.h
)

# Define export macro
target_compile_definitions(xystd PRIVATE XYSTD_EXPORTS)

# Include directories
target_include_directories(xystd PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Link with math library on Unix
if(UNIX)
    target_link_libraries(xystd m)
endif()

# Install targets
install(TARGETS xystd
    LIBRARY DESTINATION bin
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)

# Copy to bin directory after build
add_custom_command(TARGET xystd POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:xystd> ${CMAKE_BINARY_DIR}/bin/
    COMMENT "Copying xystd library to bin directory"
)

