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
    PRIVATE $<$<BOOL:${TEST_TAGS}>:TEST_TAGS="${TEST_TAGS}"> CATCH_CONFIG_DISABLE_EXCEPTIONS $<$<STREQUAL:${PLATFORM},freertos-arm>:CATCH_CONFIG_NO_POSIX_SIGNALS>
)

target_link_libraries(osal-tests
    PRIVATE osal::c osal::cpp platform::main platform::init
)

if (PLATFORM STREQUAL freertos-arm)
    objcopy_generate_bin(osal-tests)
endif ()
