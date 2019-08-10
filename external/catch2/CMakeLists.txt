cmake_minimum_required(VERSION 3.14)

add_library(catch2 INTERFACE)

target_include_directories(catch2
    INTERFACE include
)

if (PLATFORM STREQUAL "sysbios")
    # This is required, because SYS/BIOS doesn't support signals or exceptions.
    target_compile_definitions(catch2 INTERFACE CATCH_CONFIG_NO_POSIX_SIGNALS CATCH_CONFIG_DISABLE_EXCEPTIONS)
endif ()
