cmake_minimum_required(VERSION 3.16)

project(EDA
    VERSION 1.0.0
    DESCRIPTION "EDA Desktop Application"
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

# 源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    
    # 版图核心
    src/layout/layout_types.cpp
    src/layout/layout_cell.cpp
    src/layout/layout_database.cpp
    
    # 插件系统
    src/plugin/plugin_loader.cpp
    src/plugin/plugin_manager.cpp
    
    # DRC 框架
    src/drc/drc_engine.cpp
    src/drc/drc_rules.cpp
    
    # 几何库
    src/geometry/point.cpp
    src/geometry/polygon.cpp
)

set(HEADERS
    include/mainwindow.h
    
    # 版图核心
    include/layout/layout_types.h
    include/layout/layout_cell.h
    include/layout/layout_database.h
    
    # 插件系统
    include/plugin/plugin_interface.h
    include/plugin/plugin_manager.h
    include/plugin/plugin_loader.h
    
    # DRC 框架
    include/drc/drc_types.h
    include/drc/drc_engine.h
    
    # 几何库
    include/geometry/point.h
    include/geometry/polygon.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
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
set(CPACK_PACKAGE_NAME "EDA Desktop")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "EDA")
set(CPACK_NSIS_DISPLAY_NAME "EDA Desktop Application")
set(CPACK_NSIS_CONTACT "eda-team@example.com")

include(CPack)
