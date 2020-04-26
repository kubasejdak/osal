add_library(osal-linux EXCLUDE_FROM_ALL
    init.cpp
    Mutex.cpp
    Semaphore.cpp
    sleep.cpp
    Thread.cpp
    timestamp.cpp
)

target_include_directories(osal-linux
    PUBLIC ../include include
)

target_link_libraries(osal-linux
    PUBLIC pthread
)
