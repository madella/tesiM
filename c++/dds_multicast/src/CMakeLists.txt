cmake_minimum_required(VERSION 3.12.4)

if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0048 NEW)
endif()

project(DDSHelloWorld)

# Find requirements
if(NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif()

if(NOT fastrtps_FOUND)
    find_package(fastrtps REQUIRED)
endif()

# Set C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(-std=c++11)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

message(STATUS "Configuring HelloWorld publisher/subscriber example...")
file(GLOB DDS_HELLOWORLD_SOURCES_CXX "src/*.cxx")

add_executable(pub src/pub.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
target_link_libraries(pub fastrtps fastcdr)

add_executable(sub src/sub.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
target_link_libraries(sub fastrtps fastcdr)

add_executable(serv src/serv.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
target_link_libraries(serv fastrtps fastcdr)

# add_executable(backup src/backup.cpp ${DDS_HELLOWORLD_SOURCES_CXX})
# target_link_libraries(backup fastrtps fastcdr)
