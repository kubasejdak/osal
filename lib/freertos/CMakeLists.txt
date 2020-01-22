add_library(osal-freertos EXCLUDE_FROM_ALL
    mutex.cpp
)

target_include_directories(osal-freertos
    PRIVATE ../include
)
