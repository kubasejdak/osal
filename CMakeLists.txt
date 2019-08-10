cmake_minimum_required(VERSION 3.14)

include(cmake/doxygen.cmake)
include(cmake/platform.cmake)

# Define application name and settings.
set(APP osal-tests)
include(test/${APP}.cmake)

if (NOT DEFINED PLATFORM)
    message(FATAL_ERROR "'PLATFORM' is not defined!")
endif ()

# Setup platform toolchain file.
include(${platform_SOURCE_DIR}/${PLATFORM}/toolchain.cmake)

project(osal VERSION 0.1 LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE            Debug)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

# Project-wide compilation options.
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_subdirectory(${platform_SOURCE_DIR})
add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(test)

# Generate documentation.
doxygen_addTarget(${CMAKE_CURRENT_SOURCE_DIR})
