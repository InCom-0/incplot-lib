cmake_minimum_required(VERSION 3.30...4.0)

project(incplot-lib
    HOMEPAGE_URL "https://github.com/InCom-0/incplot-lib"
    DESCRIPTION
    "Library implementing the core features necessary to make inplot work."
    LANGUAGES CXX)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

#####################################################################
### Staging ###
#####################################################################
include(cmake/StageOutputs.cmake)
stageOutputDirsFor(OTFCCXX)


#####################################################################
### Platform specifics + hacks + LTO ###
#####################################################################
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
    add_compile_options(/utf-8)
    add_compile_definitions(NOMINMAX)
endif()


####################################################################
### Target specification ###
####################################################################
add_library(incplot-lib)
add_library(incplot-lib::incplot-lib ALIAS incplot-lib)

set(INCPLOT_LIB_SRC
    color_mixer.cpp
    datastore.cpp
    desired_plot.cpp
    err.cpp
    incplot.cpp
    parser_inc.cpp
    plot_structures_eval.cpp)
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

include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.
target_link_libraries(incplot-lib PRIVATE
    nlohmann_json::nlohmann_json
    csv2::csv2
    utf-cpp::utf-cpp
    otfccxx-lib
)

target_link_libraries(incplot-lib PUBLIC
    incstd::incstd
    incerr::incerr
)

if(MINGW AND CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    target_link_libraries(incplot-lib PRIVATE stdc++exp)
endif()


##########################################################################
### Subdirs - tests, demos and the like ###
##########################################################################
option(INCPLOT_BUILD_DEMOS "Build demo executables" ${PROJECT_IS_TOP_LEVEL})
if(INCPLOT_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

option(INCPLOT_BUILD_TESTS "Build test executables" ${PROJECT_IS_TOP_LEVEL})
if(INCPLOT_BUILD_TESTS)
    add_subdirectory(test)
endif()
