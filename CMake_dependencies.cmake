include(FetchContent)

# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG v1.16.0
# )
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)


FetchContent_Declare(
    glaze
    GIT_REPOSITORY https://github.com/stephenberry/glaze.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glaze)

FetchContent_Declare(nlh_json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(nlh_json)


FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/InCom-0/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse)