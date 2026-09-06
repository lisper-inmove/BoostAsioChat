enable_testing()

# add_definitions(-DUNIT_TEST)
#
# 可执行文件与库文件输出到构建目录根（即 build/test/ 下）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


message(STATUS "Add ${TEST_NAME} googletest success")

# 将源代码添加到此项目的可执行文件。
add_executable(${TEST_NAME} tests/main.cpp ${SOURCES} ${TEST_SOURCES})

# --------------------- googletest -----------------------------
file(GLOB GTEST_LIBS "${GTEST_SOURCE_DIR}/build/lib/*.lib")
target_link_libraries(${TEST_NAME} PRIVATE ${GTEST_LIBS})
target_include_directories(${TEST_NAME} PRIVATE "${GTEST_SOURCE_DIR}/googletest/include")
target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${TEST_NAME} PRIVATE ${ASIO_DIR}/include)
target_include_directories(${TEST_NAME} PRIVATE ${JSON_DIR}/include)
target_include_directories(${TEST_NAME} PRIVATE ${SPDLOG_DIR}/include)
target_include_directories(${TEST_NAME} PRIVATE ${YAML_CPP_DIR}/include)

target_link_libraries(${TEST_NAME} PRIVATE fmt::fmt)
target_link_libraries(${TEST_NAME} PRIVATE ${SPDLOG_LIB_RELEASE})
target_link_libraries(${TEST_NAME} PRIVATE ${YAML_CPP_LIB_RELEASE})

# ========== 链接库 ==========
target_link_libraries(${TEST_NAME}
    PRIVATE
    gtest_main      # GoogleTest 主函数
    gtest           # GoogleTest 核心库
    ${OpenCV_LIBS}  # OpenCV 库
)

# 在添加可执行文件后，添加测试发现
include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

target_link_libraries(${TEST_NAME} PRIVATE ${OpenCV_LIBS})

# 设置输出目录（可选）
# set_target_properties(${TEST_NAME} PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
# )

# install(TARGETS ${TEST_NAME} DESTINATION bin)

message(STATUS "Add ${TEST_NAME} test success")
