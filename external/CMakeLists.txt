set(CMAKE_FOLDER external)
list(APPEND CMAKE_MESSAGE_CONTEXT external)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source build is not supported!")
endif()

find_package(openDAQ ${SIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER})
if (NOT openDAQ_FOUND)
    add_subdirectory(openDAQ)
endif()
