include(cmake/CPM.cmake)

if(NOT CPM_USE_LOCAL_PACKAGES)
  set(CPM_USE_LOCAL_PACKAGES ON)
endif()

CPMAddPackage(URI "gh:InCom-0/otfccxx-lib#main")

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
