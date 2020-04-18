add_library(osal-linux EXCLUDE_FROM_ALL
    init.cpp
    mutex.cpp
    sleep.cpp
    thread.cpp
    timestamp.cpp
)

target_include_directories(osal-linux
    PUBLIC ../include include
)

target_link_libraries(osal-linux
    PUBLIC pthread
)
