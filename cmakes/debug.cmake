message(STATUS "debug.cmake included")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug)

# 添加可执行文件目标
add_executable(${PROJECT_NAME} "src/main.cpp" ${SOURCES})

# 指定头文件搜索路径（private 表示仅该目标使用）
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SPDLOG_LIB_DEBUG})

