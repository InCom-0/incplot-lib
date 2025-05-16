enable_testing()

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(UnitTest)
target_sources(UnitTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/unit_tests.cpp)

target_link_libraries(UnitTest PRIVATE incplot-lib gtest_main gmock_main)
