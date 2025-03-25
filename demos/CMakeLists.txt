add_executable(incplot_scratchpad ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad/scratchpad.cpp)

target_link_libraries(incplot_scratchpad PUBLIC incplot)

target_link_libraries(incplot_scratchpad PRIVATE "-lstdc++exp")


add_executable(incplot_scratchpad_dbg ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad_dbg/scratchpad_dbg.cpp)

target_link_libraries(incplot_scratchpad_dbg PUBLIC incplot)

target_link_libraries(incplot_scratchpad_dbg PRIVATE "-lstdc++exp")