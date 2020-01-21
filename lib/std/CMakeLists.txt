add_library(osal-std EXCLUDE_FROM_ALL
    init.cpp
    sleep.cpp
    timestamp.cpp
)

target_include_directories(osal-std
    PUBLIC ../include
)
