enable_testing()

# 测试可执行文件
add_executable(unit_tests
    test_main.cpp
)

target_link_libraries(unit_tests PRIVATE
    Qt5::Core
    Qt5::Test
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 添加测试
add_test(NAME UnitTests COMMAND unit_tests)
