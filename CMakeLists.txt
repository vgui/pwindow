cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(pwindow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory where exe,dll and static library saved
set(PWINDOW_BINARY_DIR ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PWINDOW_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PWINDOW_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PWINDOW_BINARY_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Examples
add_subdirectory(examples pwindow)

