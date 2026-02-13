cmake_minimum_required(VERSION 3.30...4.0)

project(incplot-lib
    HOMEPAGE_URL "https://github.com/InCom-0/incplot-lib"
    DESCRIPTION
    "Library implementing the core features necessary to make incplot work."
    LANGUAGES CXX)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(incplot-lib_BUILD_SHARED_LIB "Build a shared version of incplot-lib" ${BUILD_SHARED_LIBS})
option(incplot-lib_STAGE_OUTPUTS "Stage build output artifacts into proper directory structure" ON)

option(incplot-lib_USE_LOCAL_PACKAGES "Use local (usually system installed) packages for dependencies." ON)
option(incplot-lib_USE_LOCAL_PACKAGES_ONLY "Forbid the build system from downloading missing dependencies." OFF)

option(incplot-lib_BUILD_DEMOS "Build demo executables for incplot-lib" ${PROJECT_IS_TOP_LEVEL})
option(incplot-lib_BUILD_TESTS "Build test executables for incplot-lib" ${PROJECT_IS_TOP_LEVEL})

include(cmake/stdlibQuery.cmake)


#####################################################################
### Staging ###
#####################################################################
include(cmake/StageOutputs.cmake)
if(incplot-lib_STAGE_OUTPUTS)
    stageOutputDirsFor(incplot-lib)
endif()


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
if(incplot-lib_BUILD_SHARED_LIB)
    add_library(incplot-lib SHARED)
else()
    add_library(incplot-lib STATIC)
endif()

add_library(incplot-lib::incplot-lib ALIAS incplot-lib)

set(INCPLOT_LIB_SRC
    color_mixer.cpp
    datastore.cpp
    desired_plot.cpp
    err.cpp
    incplot.cpp
    parser_inc.cpp
    plot_structures_eval.cpp
    machinery_hash_memory_shim.cpp
)
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
    otfccxx::otfccxx
)

target_link_libraries(incplot-lib PUBLIC
    incstd::incstd
    incerr::incerr
)

#  TODO: Remove some time in the future when the exp verison of standard library is not longer necessary
if(USING_LIBSTDCXX)
    target_link_libraries(incplot-lib PRIVATE stdc++exp)
endif()

set_target_properties(incplot-lib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

if(incplot-lib_BUILD_SHARED_LIB)
    set_target_properties(incplot-lib PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
    )
    target_compile_definitions(incplot-lib
        PRIVATE INCPLOT_LIB_EXPORTS
        PUBLIC INCPLOT_LIB_SHARED
    )
endif()


##########################################################################
### Subdirs - tests, demos and the like ###
##########################################################################
if(incplot-lib_BUILD_DEMOS)
    add_subdirectory(demos)
endif()

if(incplot-lib_BUILD_TESTS)
    add_subdirectory(test)
endif()


#####################################################################
### Installing, packaging ###
#####################################################################

# TODO: INSTALLATION NOT AVAILABLE until incerr and incstd are made into installable libraries themselves

# if(BUILD_SHARED_LIBS)
#     install(
#         TARGETS incplot-lib
#         EXPORT incplot-lib_target
#         RUNTIME_DEPENDENCY_SET incplot-lib_depset
#         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#         FILE_SET pub_headers
#         DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#     )

#     # When compling with MinGW we need to include the runtime DLLs along in the package
#     if(MINGW)
#         get_filename_component(INCPLOT_LIB_CXX_COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
#         install(
#             RUNTIME_DEPENDENCY_SET incplot-lib_depset
#             PRE_EXCLUDE_REGEXES
#             [[api-ms-.*]]
#             [[ext-ms-.*]]
#             POST_INCLUDE_REGEXES
#             [[libstdc\+\+*]]
#             [[libgcc_s*]]
#             [[libwoff2*]]
#             [[libharfbuzz*]]
#             [[libxxhash*]]
#             POST_EXCLUDE_REGEXES
#             [[\\*.dll]]
#             DIRECTORIES
#             ${INCPLOT_LIB_CXX_COMPILER_DIR}
#         )
#     endif()

#     # Creating a script for being able to use 'find_package')
#     install(
#         EXPORT incplot-lib_target
#         NAMESPACE incplot-lib::
#         DESTINATION lib/cmake/incplot-lib
#     )

# else()
#     message(STATUS "Installing incplot-lib is not available because it is being built as STATIC library")
# endif(BUILD_SHARED_LIBS)
