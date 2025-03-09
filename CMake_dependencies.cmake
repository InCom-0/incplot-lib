include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    more_concepts
    GIT_REPOSITORY https://github.com/MiSo1289/more_concepts.git
    GIT_TAG origin/master
)
FetchContent_MakeAvailable(more_concepts)

FetchContent_Declare(
    functional
    GIT_REPOSITORY https://github.com/InCom-0/functional
    GIT_TAG main
)
FetchContent_MakeAvailable(functional)


find_package(fmt CONFIG REQUIRED)
find_package(glaze REQUIRED)
find_package(ctre REQUIRED)
find_package(xxHash REQUIRED)
find_package(unordered_dense REQUIRED)