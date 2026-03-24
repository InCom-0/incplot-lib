cmake_minimum_required(VERSION 3.30...4.3)
project(utf-cpp
    HOMEPAGE_URL "https://github.com/ww898/utf-cpp"
    DESCRIPTION
    "UTF-8/16/32 C++11 header only library for Windows / Linux / macOS"
    LANGUAGES CXX)

option(utfcpp_BUILD_TESTS "Build tests" FALSE)


add_library(utf-cpp INTERFACE)
add_library(utf-cpp::utf-cpp ALIAS utf-cpp)
target_compile_features(utf-cpp INTERFACE cxx_std_17)
target_sources(utf-cpp
    PUBLIC
    FILE_SET pub_headers
    TYPE HEADERS
    BASE_DIRS
    include)


if(utfcpp_BUILD_TESTS)
    add_subdirectory(test)
endif()