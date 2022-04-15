add_library(osal-common EXCLUDE_FROM_ALL
    time.cpp
    timestamp.cpp
)
add_library(osal::common ALIAS osal-common)

target_include_directories(osal-common
    PUBLIC include
    PRIVATE ../include
)

target_link_libraries(osal-common
    PUBLIC utils::logger
)
