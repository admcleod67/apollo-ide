cmake_minimum_required(VERSION 3.16)

project(
    apollo-ide
    VERSION 0.1.0
    DESCRIPTION "Cross-platform Qt IDE for Apollo compilers targeting the Gemini VM"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ApolloIdeDefaults)

option(BUILD_TESTING "Build and register tests" ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(apollo-ide
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/editorwidget.cpp
    src/editorwidget.h
    src/compileradapter.cpp
    src/compileradapter.h
    src/vmadapter.cpp
    src/vmadapter.h
    src/toolchainsettings.cpp
    src/toolchainsettings.h
    src/settingsdialog.cpp
    src/settingsdialog.h
)

target_include_directories(apollo-ide PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_definitions(apollo-ide PRIVATE
    APOLLO_IDE_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(apollo-ide PRIVATE Qt6::Widgets)

if (BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif ()
