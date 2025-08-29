cmake_minimum_required(VERSION 3.25)
project(SimpleDeviceModule VERSION 1.0.0)

set(SIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER "" CACHE STRING "Version of openDAQ SDK to build SimpleDeviceModule with")

set(REPO_OPTION_PREFIX EXAMPLE_MODULE)
set(SDK_TARGET_NAMESPACE daq)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MESSAGE_CONTEXT_SHOW ON CACHE BOOL "Show CMake message context")

option(EXAMPLE_MODULE_ENABLE_TESTS "Enable building tests for example_module" ON)
option(SIMPLE_DEVICE_MODULE_ENABLE_SERVER_APP "Enable building example server application" OFF)
if(SIMPLE_DEVICE_MODULE_ENABLE_SERVER_APP)
    set(DAQMODULES_OPENDAQ_SERVER_MODULE ON CACHE BOOL "" FORCE)
    set(OPENDAQ_ENABLE_NATIVE_STREAMING ON CACHE BOOL "" FORCE)
endif()

list(APPEND CMAKE_MESSAGE_CONTEXT ${PROJECT_NAME})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif()

add_definitions(-DFMT_HEADER_ONLY)

include(CommonUtils)
setup_repo(${REPO_OPTION_PREFIX})

if (EXAMPLE_MODULE_ENABLE_TESTS)
    message(STATUS "Unit tests are ENABLED")
    enable_testing()
endif()

add_subdirectory(external)
add_subdirectory(example_module)

if(SIMPLE_DEVICE_MODULE_ENABLE_SERVER_APP)
    message(STATUS "Enbled example server application")
    add_subdirectory(server_application)
endif()

# Set CPack variables
set(CPACK_COMPONENTS_ALL RUNTIME)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/package")

# Set the CPack generator based on the platform
if (WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif (UNIX AND NOT APPLE)
    cmake_host_system_information(RESULT DISTRO_ID QUERY DISTRIB_ID)
    cmake_host_system_information(RESULT DISTRO_VERSION_ID QUERY DISTRIB_VERSION_ID)
    set(CPACK_SYSTEM_NAME "${DISTRO_ID}${DISTRO_VERSION_ID}")
    set(CPACK_GENERATOR "TGZ")
endif()

# Include CPack for packaging
include(CPack)
