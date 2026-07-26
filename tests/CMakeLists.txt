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

add_executable(compileradapter_test
    compileradapter_test.cpp
    ../src/compileradapter.cpp
    ../src/compileradapter.h
)

target_include_directories(compileradapter_test PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
)

target_compile_definitions(compileradapter_test PRIVATE
    APOLLO_TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

target_link_libraries(compileradapter_test PRIVATE
    Qt6::Widgets
    Qt6::Test
)

add_test(NAME compileradapter_test COMMAND compileradapter_test)
