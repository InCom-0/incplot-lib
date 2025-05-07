cmake_minimum_required(VERSION 3.30...4.0)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

project(incplot)

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.


add_library(incplot-lib STATIC)

target_sources(incplot-lib PRIVATE src/incplot.cpp src/PD.cpp src/desired_plot_impl.cpp src/datastore_impl.cpp)
add_library(incplot-lib::incplot-lib ALIAS incplot-lib)
target_include_directories(incplot-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(incplot-lib PUBLIC cxx_std_23)

target_sources(incplot-lib
    PRIVATE
        FILE_SET priv_headers
        TYPE HEADERS
        BASE_DIRS
                src/private_inc)


target_link_libraries(incplot-lib PUBLIC
    nlohmann_json::nlohmann_json
    csv2::csv2
    argparse::argparse
    utf-cpp::utf-cpp
    magic_enum::magic_enum)



option(INCPLOT_BUILD_DEMOS "Build demo executables" ${PROJECT_IS_TOP_LEVEL})


if(INCPLOT_BUILD_DEMOS)
    add_subdirectory(demos)
endif()
