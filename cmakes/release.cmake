message(STATUS "release.cmake included")

# 可执行文件与库文件输出到 build/bin 和 build/lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/release)

# 添加可执行文件目标
add_executable(${PROJECT_NAME} "src/main.cpp" ${SOURCES})

# 指定头文件搜索路径（private 表示仅该目标使用）
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
