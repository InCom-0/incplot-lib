if(NOT DEFINED CPM_USE_LOCAL_PACKAGES)
    set(CPM_USE_LOCAL_PACKAGES ${incplot-lib_USE_LOCAL_PACKAGES} CACHE BOOL "CPM will try to find packages locally first" FORCE)
endif()
if(NOT DEFINED CPM_LOCAL_PACKAGES_ONLY)
    set(CPM_LOCAL_PACKAGES_ONLY ${incplot-lib_USE_LOCAL_PACKAGES_ONLY} CACHE BOOL
        "CPM will not be forbidden from downloading packages. Will have to use local packages." FORCE)
endif()

include(cmake/CPM.cmake)


CPMAddPackage(
    URI "gh:InCom-0/otfccxx#main"
    OPTIONS "otfccxx_BUILD_SHARED_LIB ${incplot-lib_BUILD_SHARED_LIB}"
    NAME otfccxx
)

CPMAddPackage(
  NAME nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
  URL_HASH SHA256=42f6e95cad6ec532fd372391373363b62a14af6d771056dbfc86160e6dfff7aa
  EXCLUDE_FROM_ALL TRUE
)

CPMAddPackage("gh:p-ranav/csv2#master")
CPMAddPackage("gh:InCom-0/incstd#main")
CPMAddPackage("gh:InCom-0/incerr#main")
CPMAddPackage("gh:InCom-0/utf-cpp#master")
