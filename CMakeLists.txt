cmake_minimum_required(VERSION 3.30)

set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)
set(VCPKG_HOST_TRIPLET x64-mingw-dynamic)

project(incplot)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMake_dependencies.cmake)  ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.

add_library(incplot INTERFACE)
target_sources(incplot
    INTERFACE FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(demos)