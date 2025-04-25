include(FetchContent)

# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG v1.16.0
# )
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
    nlh_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(nlh_json)


FetchContent_Declare(
    csv2
    GIT_REPOSITORY https://github.com/p-ranav/csv2.git
    GIT_TAG master
)
FetchContent_MakeAvailable(csv2)


FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse)
