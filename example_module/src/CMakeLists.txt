set(LIB_NAME example_module)
set(MODULE_HEADERS_DIR ../include/${TARGET_FOLDER_NAME})
list(APPEND CMAKE_MESSAGE_CONTEXT ${LIB_NAME})

set(SRC_Include common.h
                module_dll.h
                example_module.h
                example_device.h
                example_channel.h
)

set(SRC_Srcs module_dll.cpp
             example_module.cpp
             example_device.cpp
             example_channel.cpp
)

prepend_include(${TARGET_FOLDER_NAME} SRC_Include)

source_group("module" FILES ${MODULE_HEADERS_DIR}/example_module.h
                            ${MODULE_HEADERS_DIR}/example_channel.h
                            ${MODULE_HEADERS_DIR}/example_device.h
                            ${MODULE_HEADERS_DIR}/module_dll.h
                            module_dll.cpp
                            example_module.cpp
                            example_device.cpp
                            example_channel.cpp
)


add_library(${LIB_NAME} SHARED ${SRC_Include}
                               ${SRC_Srcs}
)

add_library(${SDK_TARGET_NAMESPACE}::${LIB_NAME} ALIAS ${LIB_NAME})

if (MSVC)
    target_compile_options(${LIB_NAME} PRIVATE /bigobj)
endif()

find_package(openDAQ ${SIMPLE_DEVICE_MODULE_OPENDAQ_SDK_VER} REQUIRED)
target_link_libraries(${LIB_NAME} PUBLIC daq::opendaq
)

target_include_directories(${LIB_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                                              $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
                                              $<INSTALL_INTERFACE:include>
)

opendaq_set_module_properties(${LIB_NAME} ${PROJECT_VERSION_MAJOR})
create_version_header(${LIB_NAME})
