cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

project(linda-app)

if (UNIX)
    include_directories("include")

    add_subdirectory(lib/googletest)
    include_directories(lib/googletest include)

    add_subdirectory(lib/fmt)
    include_directories(lib/fmt/include)

    add_subdirectory(lib/loguru)
    include_directories(lib/loguru)

    file (GLOB lindux_sources
        "include/*.hpp"
        "src/*.cpp"  
    )

    file (GLOB lindux_unit_test_sources
        "test/unit/*.cpp" 
    )

    add_library(lindux
        ${lindux_sources}
    )
    target_link_libraries(lindux fmt loguru pthread dl)

    add_executable(unit_test ${lindux_unit_test_sources})
    add_executable(hello-lindux test/apps/hello_lindux.cpp)

    target_link_libraries(unit_test lindux gtest gtest_main)
    target_link_libraries(hello-lindux lindux)
elseif (WIN32)
    message(ERROR "Windows not supported!")
endif ()
