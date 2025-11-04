cmake_minimum_required(VERSION 3.30...4.0)

project(incplot-lib)
# set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/utf-8)
endif()

add_library(incplot-lib STATIC)
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

### This attempts to compile sample program to find out if we are using libc++(USING_LIBCXX) or libstdc++(USING_LIBSTDCXX)
include(cmake/stdlibQuery.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(STATUS "GCC Compiler")
        message(STATUS "Using libstdc++")
        target_link_options(incplot-lib PUBLIC -static -static-libgcc -static-libstdc++)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        message(STATUS "Clang Compiler")
        if(USING_LIBCXX)
            message(STATUS "Using libc++")
            target_link_options(incplot-lib PUBLIC -static -stdlib=libc++ -static-libstdc++)
        elseif(USING_LIBSTDCXX)
            message(STATUS "Using libstdc++")
            target_link_options(incplot-lib PUBLIC -static -static-libgcc -static-libstdc++)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message(STATUS "MSVC Compiler")
    else()
        message(WARNING "You are using an unexpected compiler (ie. neither GCC, Clang, AppleClang, MSVC). All bets are off.")
    endif()
endif()


include(CMake_dependencies.cmake) ### Loads and declares the requires external libraries using both vcpkg and CMake's FetchContent.
target_link_libraries(incplot-lib PRIVATE
    nlohmann_json::nlohmann_json
    csv2::csv2
    utf-cpp::utf-cpp)


target_link_libraries(incplot-lib PUBLIC
    magic_enum::magic_enum
    incstd::incstd
    incerr::incerr)


option(INCPLOT_BUILD_DEMOS "Build demo executables" ${PROJECT_IS_TOP_LEVEL})
if(INCPLOT_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

option(INCPLOT_BUILD_TESTS "Build test executables" ${PROJECT_IS_TOP_LEVEL})
if(INCPLOT_BUILD_TESTS)
    add_subdirectory(test)
endif()
