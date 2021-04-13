cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

project(linda-app)

if (UNIX)
    include_directories("include")
    file (GLOB lindux_sources
        "include/*.hpp"
        "src/*.cpp"  
    )
    add_library(lindux
        ${lindux_sources}
    )
    # target_link_libraries(lindux)
    add_executable(hello-lindux test/apps/hello_lindux.cpp)
    target_link_libraries(hello-lindux lindux)
elseif (WIN32)
    message(ERROR "Windows not supported!")
endif ()
