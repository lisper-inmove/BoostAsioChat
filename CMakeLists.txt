# =============================================================================
# 基础配置
# =============================================================================

cmake_minimum_required(VERSION 3.16)

# 定义项目名称、版本号、使用的语言（CXX 表示 C++）
project(ChatServer VERSION 1.0.0 LANGUAGES CXX)

# =============================================================================
# 构建选项
# =============================================================================

# 可选：允许用户切换构建静态库还是动态库（默认静态）
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# 可选：是否启用测试（默认 OFF）
option(ENABLE_TESTS "Enable unit tests" OFF)

# =============================================================================
# C++ 标准设置
# =============================================================================

# 设置 C++ 标准为 C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# 输出目录（便于管理生成文件）
# =============================================================================

# =============================================================================
# 源文件与头文件
# =============================================================================

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)   # 假设测试代码在 tests/ 子目录
endif()

message(STATUS "Project: ${PROJECT_NAME} version ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build shared libs: ${BUILD_SHARED_LIBS}")

include(cmakes/main.cmake)
