add_executable(incplot_scratchpad_dbg ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad_dbg/scratchpad_dbg.cpp)
target_link_libraries(incplot_scratchpad_dbg PRIVATE incplot-lib "-lstdc++exp")
target_compile_features(incplot_scratchpad_dbg PRIVATE cxx_std_23)

target_sources(incplot_scratchpad_dbg
    PUBLIC
        FILE_SET pub_headers
        TYPE HEADERS
        BASE_DIRS
                ${incplot_SOURCE_DIR}/test/inc/)