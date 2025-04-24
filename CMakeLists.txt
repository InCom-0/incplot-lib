cmake_minimum_required(VERSION 3.30)

project(incplot)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.


add_library(incplot INTERFACE)
target_include_directories(incplot INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(incplot INTERFACE cxx_std_23)


target_link_libraries(incplot INTERFACE
    nlohmann_json::nlohmann_json
    glaze::glaze
    argparse::argparse)



option(INCPLOT_BUILD_DEMOS "Build demo executables" ${PROJECT_IS_TOP_LEVEL})


if (INCPLOT_BUILD_DEMOS)
    add_subdirectory(demos)
endif()