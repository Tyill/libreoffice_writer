cmake_minimum_required(VERSION 3.0.2)

project(reoffice_writer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Builds/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Builds/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Builds/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME})


if (UNIX)
  add_compile_definitions(LINUX)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif(UNIX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/tools)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/rtl)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sw/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sw/source/core/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config_host)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/udkapi)

FILE(GLOB_RECURSE src "*.cpp" "*.cxx" "*.h" "*.hxx")

add_library(reoffice_writer SHARED ${src})