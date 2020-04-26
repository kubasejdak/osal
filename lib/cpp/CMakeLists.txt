add_library(osal-cpp
    Error.cpp
    init.cpp
    Mutex.cpp
    ScopedLock.cpp
    Semaphore.cpp
    sleep.cpp
    timestamp.cpp
)

target_include_directories(osal-cpp
    PUBLIC include
)

target_link_libraries(osal-cpp
    PUBLIC osal-c
)
