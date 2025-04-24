add_executable(incplot_scratchpad ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad/scratchpad.cpp)
target_link_libraries(incplot_scratchpad PRIVATE incplot "-lstdc++exp")
target_compile_features(incplot_scratchpad PRIVATE cxx_std_23)


add_executable(incplot_scratchpad_dbg ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad_dbg/scratchpad_dbg.cpp)
target_link_libraries(incplot_scratchpad_dbg PRIVATE incplot "-lstdc++exp")
target_compile_features(incplot_scratchpad PRIVATE cxx_std_23)