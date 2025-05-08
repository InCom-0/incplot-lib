cmake_minimum_required(VERSION 3.30...4.0)

project(incplot)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)


add_library(incplot-lib STATIC)
add_library(incplot-lib::incplot-lib ALIAS incplot-lib)

target_sources(incplot-lib PRIVATE src/incplot.cpp src/desired_plot.cpp src/datastore.cpp src/color_mixer.cpp src/parser_inc.cpp src/args.cpp src/cross_platform.cpp)
target_sources(incplot-lib
    PUBLIC
        FILE_SET pub_headers
        TYPE HEADERS
        BASE_DIRS
            include)
target_sources(incplot-lib
    PRIVATE
        FILE_SET priv_headers
        TYPE HEADERS
        BASE_DIRS
                src/private_inc)

target_compile_features(incplot-lib PUBLIC cxx_std_23)


include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.
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
