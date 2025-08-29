get_current_folder_name(TARGET_FOLDER_NAME)
project(ExampleModule VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(src)

if (EXAMPLE_MODULE_ENABLE_TESTS)
    add_subdirectory(tests)
endif()
