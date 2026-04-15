add_executable(list_example
    main.cpp
)

target_link_libraries(list_example PRIVATE
    list_lib
)

add_custom_command(TARGET list_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/inlet.in
        $<TARGET_FILE_DIR:list_example>/inlet.in
)

add_test(NAME list_example_runs COMMAND list_example)