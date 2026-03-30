enable_testing()

# 获取 GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# 测试可执行文件
add_executable(unit_tests
    test_main.cpp
    test_gds_parser.cpp
    test_layout_core.cpp
)

target_link_libraries(unit_tests PRIVATE
    Qt5::Core
    Qt5::Test
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 添加测试
add_test(NAME UnitTests COMMAND unit_tests)

# 启用测试发现
include(GoogleTest)
gtest_discover_tests(unit_tests)
