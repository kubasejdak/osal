# Object library type is used to prevent compiler from removing unit test functions from the binary.
add_library(apposal-tests OBJECT EXCLUDE_FROM_ALL
    appMain.cpp
)

target_include_directories(apposal-tests
    PRIVATE .
)

target_compile_features(apposal-tests
    PRIVATE cxx_std_17
)

target_link_libraries(apposal-tests
    PRIVATE catch2
)

# This is required to selectively run Catch2 tests using tags on platforms, where cmd line arguments are not available.
if (DEFINED TEST_TAGS)
    target_compile_definitions(apposal-tests
        PRIVATE TEST_TAGS="${TEST_TAGS}"
    )
endif ()
