cmake_minimum_required(VERSION 3.30)

project(incplot)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.


add_library(incplot INTERFACE)
target_include_directories(incplot INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(incplot PROPERTIES
    LINKER_LANGUAGE CXX
    CXX_STANDARD 23)


target_link_libraries(incplot INTERFACE
    nlohmann_json::nlohmann_json
    glaze::glaze
    argparse::argparse)

target_link_options(incplot INTERFACE -static-libgcc -static-libstdc++ "LINKER:-Bstatic,--whole-archive" -lwinpthread "LINKER:-Bdynamic,--no-whole-archive")

add_subdirectory(demos)
