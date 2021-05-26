cmake_minimum_required(VERSION 3.11)
include(FetchContent)

FetchContent_Declare(stduuid
  GIT_REPOSITORY https://github.com/mariusbancila/stduuid
)
FetchContent_GetProperties(stduuid)
if(NOT stduuid_POPULATED)
  FetchContent_Populate(stduuid)
  add_subdirectory(${stduuid_SOURCE_DIR} ${stduuid_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pthread -Wpedantic ")

project(linda-app)

if (UNIX)
    include_directories("include")
    set(SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/src/)
    set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin/)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
    # libraries
    add_subdirectory(lib/googletest)
    include_directories(lib/googletest include)

    add_subdirectory(lib/fmt)
    include_directories(lib/fmt/include)

    add_subdirectory(lib/loguru)
    include_directories(lib/loguru)
    # sources
    file (GLOB lindux_sources
            "include/*.hpp"
            "src/*.cpp"
            )

    file (GLOB lindux_headers
            "include/*.hpp")

    file (GLOB lindux_unit_test_sources
            "test/unit/*.cpp"
            )
    # project library
    add_library(lindux
            ${lindux_sources}
            )
    target_link_libraries(lindux fmt loguru pthread dl stduuid)
    # executables
    add_executable(server test/apps/server_main.cpp ${lindux_headers})
    add_executable(client test/apps/client_main.cpp)
    add_executable(unit_test ${lindux_unit_test_sources})
    add_executable(hello-lindux test/apps/hello_lindux.cpp)
    target_link_libraries(server lindux)
    target_link_libraries(client lindux)
    target_link_libraries(unit_test lindux gtest gtest_main)
    target_link_libraries(hello-lindux lindux)
elseif (WIN32)
    message(ERROR "Windows not supported!")
endif ()
