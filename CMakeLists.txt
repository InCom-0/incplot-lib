cmake_minimum_required(VERSION 3.30...4.0)

project(incplot)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)


add_library(incplot-lib STATIC)
add_library(incplot-lib::incplot-lib ALIAS incplot-lib)

set(INCPLOT_LIB_SRC
    incplot.cpp
    desired_plot.cpp
    datastore.cpp
    color_mixer.cpp
    parser_inc.cpp
    args.cpp)
list(TRANSFORM INCPLOT_LIB_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_sources(incplot-lib PRIVATE ${INCPLOT_LIB_SRC})
target_sources(incplot-lib
    PUBLIC
        FILE_SET pub_headers
        TYPE HEADERS
        BASE_DIRS
                include/L2/)
target_sources(incplot-lib
    PRIVATE
        FILE_SET priv_headers
        TYPE HEADERS
        BASE_DIRS
                src/private_inc/)
                
target_compile_features(incplot-lib PUBLIC cxx_std_23)

if(MINGW AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
    target_link_options(incplot-lib PUBLIC -static -static-libgcc -static-libstdc++)
endif()

if((CMAKE_SYSTEM_NAME STREQUAL "Darwin") AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
  target_link_options(incplot-lib PUBLIC -static -static-libgcc -static-libstdc++)
endif()



include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.
target_link_libraries(incplot-lib PRIVATE
    nlohmann_json::nlohmann_json
    csv2::csv2
    utf-cpp::utf-cpp
    incstd::incstd)

target_link_libraries(incplot-lib PUBLIC
    argparse::argparse
    magic_enum::magic_enum
    incerr::incerr)



option(INCPLOT_BUILD_DEMOS "Build demo executables" ${PROJECT_IS_TOP_LEVEL})
if(INCPLOT_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

option(INCPLOT_BUILD_TESTS "Build test executables" ${PROJECT_IS_TOP_LEVEL})
if(INCPLOT_BUILD_TESTS)
    add_subdirectory(test)
endif()
