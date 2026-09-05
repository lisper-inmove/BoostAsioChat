message(STATUS "server.cmake included")

set(THIRD_PARTY_ROOT "/home/inmove/nvme1/SourceCodes")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TEST_NAME "ChatServerTest")
set(GTEST_SOURCE_DIR "${THIRD_PARTY_ROOT}/googletest")
set(ASIO_DIR "${THIRD_PARTY_ROOT}/asio")
set(JSON_DIR "${THIRD_PARTY_ROOT}/json")
set(SPDLOG_DIR "${THIRD_PARTY_ROOT}/spdlog")
# spdlog 2.0 是「编译库」模式（无 header-only），需链接预编译静态库，Debug/Release 分开
set(SPDLOG_LIB_RELEASE "${SPDLOG_DIR}/build/libspdlog.a")
set(SPDLOG_LIB_DEBUG "${SPDLOG_DIR}/build/libspdlog-2.0d.a")

# fmt 已编译安装到 /usr/local。注意：系统里还有 conda 的 fmt（11.2.0）会干扰
# find_package 的搜索（conda 的 bin 在 PATH 里，被 CMake 推导为搜索前缀）。
# 这里显式固定到 /usr/local 的 CMake 配置，确保用 12.2.1 而不是 conda 的旧版本。
set(fmt_DIR "/usr/local/lib/cmake/fmt" CACHE PATH "fmt CMake config dir" FORCE)
find_package(fmt CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cc")
message(STATUS "sources files ${SOURCES}")
file(GLOB_RECURSE TEST_SOURCES "tests/*.cc")
message(STATUS "test sources files ${TEST_SOURCES}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  include(cmakes/debug.cmake)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  include(cmakes/release.cmake)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Test")
  include(cmakes/googletest.cmake)
else()
  target_include_directories(${PROJECT_NAME} PRIVATE ${ASIO_DIR}/include ${JSON_DIR}/include ${SPDLOG_DIR}/include)
  target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
endif()

