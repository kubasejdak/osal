add_library(osal-freertos EXCLUDE_FROM_ALL
    init.cpp
    Mutex.cpp
    Semaphore.cpp
    sleep.cpp
    Thread.cpp
    timestamp.cpp
)

target_include_directories(osal-freertos
    PUBLIC ../include include
)

target_link_libraries(osal-freertos
    PUBLIC freertos
)
