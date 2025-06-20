FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

set(INCPLOT_LIB_TEST_SRC
    parser_test.cpp
    dp_guess_test.cpp)

list(TRANSFORM INCPLOT_LIB_TEST_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)


add_executable(UnitTest)
target_sources(UnitTest PRIVATE ${INCPLOT_LIB_TEST_SRC})

target_sources(UnitTest
    PUBLIC
        FILE_SET priv_headers
        TYPE HEADERS
        BASE_DIRS
                inc/)

target_link_libraries(UnitTest PRIVATE incplot-lib incstd::incstd gtest_main gmock_main "-lstdc++exp")
