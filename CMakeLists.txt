cmake_minimum_required(VERSION 3.16)

project(EDA
    VERSION 1.0.0
    DESCRIPTION "EDA Layout Editor"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找 Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# UI 源文件
set(UI_SOURCES
    src/ui/mainwindow.cpp
    src/ui/layout_canvas.cpp
    src/ui/project_tree.cpp
    src/ui/layer_panel.cpp
    src/ui/property_editor.cpp
)

set(UI_HEADERS
    include/ui/mainwindow.h
    include/ui/layout_canvas.h
    include/ui/project_tree.h
    include/ui/layer_panel.h
    include/ui/property_editor.h
)

# 资源文件
set(RESOURCES
    resources/styles/dark.qss
    resources/resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    ${UI_SOURCES}
    ${UI_HEADERS}
    ${RESOURCES}
)

# 链接 Qt5
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 测试
enable_testing()
add_subdirectory(tests)

# 版本信息
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR})

# CPack 打包配置
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "EDA Layout Editor")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "EDA")
set(CPACK_NSIS_DISPLAY_NAME "EDA Layout Editor")
set(CPACK_NSIS_CONTACT "eda-team@example.com")

include(CPack)
