set(SERVER_APP_NAME test_${MODULE_NAME})
list(APPEND CMAKE_MESSAGE_CONTEXT ${SERVER_APP_NAME})

add_executable(server_example main.cpp)

target_link_libraries(server_example PRIVATE daq::opendaq)

add_dependencies(
    example_module
    daq::native_stream_srv_module
)
