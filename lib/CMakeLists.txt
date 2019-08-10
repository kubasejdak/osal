if (NOT DEFINED OSAL_PLATFORM)
    message(FATAL_ERROR "'OSAL_PLATFORM' is not defined!")
endif ()

add_subdirectory(${OSAL_PLATFORM})
