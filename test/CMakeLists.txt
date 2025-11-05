CPMAddPackage("gh:google/googletest@1.17.0")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
enable_testing()

if((TARGET gtest) AND (CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU"))
    target_compile_options(gtest PRIVATE -Wno-character-conversion)
endif()


set(INCPLOT_LIB_TEST_SRC
    parser_test.cpp
    dp_guess_test.cpp
    ps_test.cpp
    ps_scatter_test.cpp
    ps_multiline_test.cpp
    ps_barv_test.cpp
    ps_barh_test.cpp)

list(TRANSFORM INCPLOT_LIB_TEST_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)


add_executable(UnitTest)
target_sources(UnitTest PRIVATE ${INCPLOT_LIB_TEST_SRC})

target_sources(UnitTest
    PUBLIC
    FILE_SET priv_headers
    TYPE HEADERS
    BASE_DIRS
    inc/)

target_link_libraries(UnitTest PRIVATE incplot-lib incstd::incstd gtest_main gmock_main)
if(USING_LIBSTDCXX)
    target_link_libraries(UnitTest PRIVATE "-lstdc++exp")
endif()
