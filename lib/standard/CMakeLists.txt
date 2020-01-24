add_library(osal-standard EXCLUDE_FROM_ALL
    init.cpp
    sleep.cpp
    timestamp.cpp
)

target_include_directories(osal-standard
    PUBLIC ../include
)
