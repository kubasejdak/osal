add_subdirectory(init)

# Object library type is used to prevent compiler from removing unit test functions from the binary.
add_library(apposal-tests OBJECT EXCLUDE_FROM_ALL
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
)

target_include_directories(apposal-tests
    PRIVATE .
)

conan_get(REQUIRES catch2/2.13.7)
find_package(Catch2 REQUIRED)

target_compile_definitions(apposal-tests
    # This is required to selectively run Catch2 tests using tags on platforms, where cmd line arguments are not available.
    PRIVATE $<$<BOOL:${TEST_TAGS}>:TEST_TAGS="${TEST_TAGS}">
)

target_compile_definitions(Catch2::Catch2
    INTERFACE CATCH_CONFIG_DISABLE_EXCEPTIONS $<$<STREQUAL:${PLATFORM},freertos-arm>:CATCH_CONFIG_NO_POSIX_SIGNALS>
)

target_link_libraries(apposal-tests
    PUBLIC platform-init
    PRIVATE Catch2::Catch2 osal::c osal::cpp
)
