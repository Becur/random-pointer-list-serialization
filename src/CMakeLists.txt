add_library(list_lib
    list.cpp
    list_serializer.cpp
)

target_include_directories(list_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)