cmake_minimum_required(VERSION 3.30)

set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)
set(VCPKG_HOST_TRIPLET x64-mingw-dynamic)

project(incplot)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.

add_library(incplot STATIC ${CMAKE_CURRENT_SOURCE_DIR}/include/incplot.hpp)
target_include_directories(incplot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(incplot PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(incplot PUBLIC
    OOF::OOF
    nlohmann_json::nlohmann_json
    more_concepts::more_concepts
    DataFrame::DataFrame)


add_subdirectory(demos)
