add_library(osal-freertos EXCLUDE_FROM_ALL
    init.cpp
    mutex.cpp
    sleep.cpp
    timestamp.cpp
)

target_include_directories(osal-freertos
    PUBLIC ../include/c
)

target_link_libraries(osal-freertos
    PRIVATE freertos
)
