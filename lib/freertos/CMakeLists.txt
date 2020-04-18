add_library(osal-freertos EXCLUDE_FROM_ALL
    init.cpp
    mutex.cpp
    sleep.cpp
    thread.cpp
    timestamp.cpp
)

target_include_directories(osal-freertos
    PUBLIC ../include include
)

target_link_libraries(osal-freertos
    PUBLIC freertos
)
