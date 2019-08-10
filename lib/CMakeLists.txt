if (NOT DEFINED OSAL_PLATFORM)
    message(FATAL_ERROR "'OSAL_PLATFORM' is not defined!")
endif ()

# Project-wide compilation options.
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

add_subdirectory(${OSAL_PLATFORM})

add_library(osal EXCLUDE_FROM_ALL
    Error.cpp
    Timeout.cpp
)

target_include_directories(osal
    PUBLIC include
)

target_link_libraries(osal
    INTERFACE osal-${OSAL_PLATFORM}
)
