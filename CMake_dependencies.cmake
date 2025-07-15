include(FetchContent)

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
    incstd
    GIT_REPOSITORY https://github.com/InCom-0/incstd
    GIT_TAG main
)
FetchContent_MakeAvailable(incstd)

FetchContent_Declare(
    incerr
    GIT_REPOSITORY https://github.com/InCom-0/incerr
    GIT_TAG main
)
FetchContent_MakeAvailable(incerr)

FetchContent_Declare(
    utf-cpp
    GIT_REPOSITORY https://github.com/InCom-0/utf-cpp
    GIT_TAG master
)
FetchContent_MakeAvailable(utf-cpp)

FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG master
)
FetchContent_MakeAvailable(argparse)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
    cppcoro
    GIT_REPOSITORY https://github.com/andreasbuhr/cppcoro.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(cppcoro)

# FetchContent_Declare (
#     tracy
#     GIT_REPOSITORY https://github.com/wolfpld/tracy.git
#     GIT_TAG master
#     GIT_SHALLOW TRUE
#     GIT_PROGRESS FALSE
# )
# FetchContent_MakeAvailable(tracy)