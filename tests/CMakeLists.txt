find_package(Qt6 REQUIRED COMPONENTS Widgets Test)

add_executable(editorwidget_test
    editorwidget_test.cpp
    ../src/editorwidget.cpp
    ../src/editorwidget.h
)

target_include_directories(editorwidget_test PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

target_link_libraries(editorwidget_test PRIVATE
    Qt6::Widgets
    Qt6::Test
)

add_test(NAME editorwidget_test COMMAND editorwidget_test)
