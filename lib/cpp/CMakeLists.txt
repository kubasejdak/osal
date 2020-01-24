add_library(osal-cpp
    init.cpp
    sleep.cpp
    timestamp.cpp
)

target_include_directories(osal-cpp
    PUBLIC include
)

target_link_libraries(osal-cpp
    PRIVATE osal-c
)
