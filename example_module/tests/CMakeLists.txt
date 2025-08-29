set(MODULE_NAME example_module)
set(TEST_APP test_${MODULE_NAME})
list(APPEND CMAKE_MESSAGE_CONTEXT ${TEST_APP})

set(TEST_SOURCES test_example_module.cpp
                 test_app.cpp
)

add_executable(${TEST_APP} ${TEST_SOURCES}
)

find_package(openDAQ ${SIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER} REQUIRED)
target_link_libraries(${TEST_APP} PRIVATE daq::opendaq
                                          ${SDK_TARGET_NAMESPACE}::${MODULE_NAME}
)

if (NOT TARGET gtest)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

target_link_libraries(${TEST_APP} 
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
)

add_test(NAME ${TEST_APP}
         COMMAND $<TARGET_FILE_NAME:${TEST_APP}>
         WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_APP}>
)

set_target_properties(${TEST_APP} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_APP}>)
