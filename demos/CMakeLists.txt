add_executable(incplot_scratchpad ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad/scratchpad.cpp)
target_sources(incplot
    INTERFACE FILE_SET HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/scratchpad)

    target_link_libraries(incplot_scratchpad PRIVATE incplot)