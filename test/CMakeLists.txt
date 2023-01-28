find_library(Catch2 NAMES Catch2 Catch2d REQUIRED)

add_subdirectory(init)

add_executable(osal-tests
    appMain.cpp
    Error.cpp
    Mutex.cpp
    MutexObject.cpp
    ScopedLock.cpp
    Semaphore.cpp
    SemaphoreObject.cpp
    Thread.cpp
    ThreadObject.cpp
    time.cpp
    Timeout.cpp
    timestamp.cpp
)

target_compile_definitions(osal-tests
    # This is required to selectively run Catch2 tests using tags on platforms, where cmd line arguments are not available.
    PRIVATE $<$<BOOL:${TEST_TAGS}>:TEST_TAGS="${TEST_TAGS}">
)

target_link_libraries(osal-tests
    PRIVATE optimized Catch2 debug Catch2d osal::c osal::cpp platform::init platform::main
)

if (PLATFORM STREQUAL freertos-arm)
    objcopy_generate_bin(osal-tests)
endif ()
