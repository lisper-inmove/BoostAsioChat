message(STATUS "server.cmake included")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(TEST_NAME "ChatServerTest")
set(GTEST_SOURCE_DIR "/home/inmove/nvme1/SourceCodes/googletest")
set(ASIO_DIR "/home/inmove/nvme1/SourceCodes/asio")
set(JSON_DIR "/home/inmove/nvme1/SourceCodes/json")

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
  target_include_directories(${PROJECT_NAME} PRIVATE ${ASIO_DIR}/include ${JSON_DIR}/include)
endif()

