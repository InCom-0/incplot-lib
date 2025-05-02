cmake_minimum_required(VERSION 3.30...4.0)

project(incplot)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.


add_library(incplot-lib INTERFACE)
add_library(incplot-lib::incplot-lib ALIAS incplot-lib)
target_include_directories(incplot-lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(incplot-lib INTERFACE cxx_std_23)


target_link_libraries(incplot-lib INTERFACE
    nlohmann_json::nlohmann_json
    csv2::csv2
    argparse::argparse
    utf-cpp::utf-cpp)



option(INCPLOT_BUILD_DEMOS "Build demo executables" ${PROJECT_IS_TOP_LEVEL})


if (INCPLOT_BUILD_DEMOS)
    add_subdirectory(demos)
endif()