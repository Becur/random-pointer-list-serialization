add_executable(list_tests
    tests.cpp
)

target_link_libraries(list_tests PRIVATE
    list_lib
)

target_include_directories(list_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(NAME list_tests COMMAND list_tests)