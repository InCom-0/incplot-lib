cmake_minimum_required(VERSION 3.30)

set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)
set(VCPKG_HOST_TRIPLET x64-mingw-dynamic)

project(incplot)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.

add_library(incplot INTERFACE)
target_include_directories(incplot INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(incplot PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(incplot INTERFACE
    nlohmann_json::nlohmann_json
    glaze::glaze)

add_subdirectory(demos)
