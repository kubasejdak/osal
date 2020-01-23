add_library(osal-common EXCLUDE_FROM_ALL
    init.cpp
    sleep.cpp
)

target_include_directories(osal-common
    PUBLIC ../include/cpp

)

target_link_libraries(osal-common
    PUBLIC osal-c
)
