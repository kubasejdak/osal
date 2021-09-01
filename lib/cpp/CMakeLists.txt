add_library(osal-cpp EXCLUDE_FROM_ALL
    Error.cpp
    init.cpp
    Mutex.cpp
    ScopedLock.cpp
    Semaphore.cpp
    sleep.cpp
    timestamp.cpp
)
add_library(osal::cpp ALIAS osal-cpp)

target_include_directories(osal-cpp
    PUBLIC include
)

target_link_libraries(osal-cpp
    PUBLIC osal::c
    PRIVATE osal::common
)
