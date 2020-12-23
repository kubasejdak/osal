cmake_minimum_required(VERSION 3.19)

if (NOT DEFINED OSAL_PLATFORM)
    message(FATAL_ERROR "'OSAL_PLATFORM' is not defined!")
endif ()

# Project-wide compilation options.
add_compile_options(-Wall -Wextra -Wpedantic -Werror $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

add_subdirectory(${OSAL_PLATFORM})
add_subdirectory(common)
add_subdirectory(cpp)

# OSAL for C.
add_library(osal-c INTERFACE)
add_library(osal::c ALIAS osal-c)

target_link_libraries(osal-c
    INTERFACE osal-${OSAL_PLATFORM} osal-common
)

# OSAL for C++.
add_library(osal::cpp ALIAS osal-cpp)
