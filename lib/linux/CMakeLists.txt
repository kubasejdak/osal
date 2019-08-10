add_library(osal-linux EXCLUDE_FROM_ALL
    Mutex.cpp
)

target_include_directories(osal-linux
    PRIVATE ../include
)

target_link_libraries(osal-linux
    PRIVATE pthread
)
