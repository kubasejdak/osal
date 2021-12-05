cmake_minimum_required(VERSION 3.21)

if (NOT DEFINED OSAL_PLATFORM)
    message(FATAL_ERROR "'OSAL_PLATFORM' is not defined!")
endif ()

# Project-wide compilation options.
add_compile_options(-Wall -Wextra -Wpedantic -Werror $<$<COMPILE_LANGUAGE:CXX>:-std=c++17> $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

# OSAL for C.
add_library(osal-c EXCLUDE_FROM_ALL)
add_library(osal::c ALIAS osal-c)

target_include_directories(osal-c
    PUBLIC include ${OSAL_PLATFORM}/include
)

target_link_libraries(osal-c
    PRIVATE osal::common
)

add_subdirectory(${OSAL_PLATFORM})
add_subdirectory(common)
add_subdirectory(cpp)
