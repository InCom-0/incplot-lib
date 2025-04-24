
add_executable(make_single_header make_single_header.cpp)
target_compile_features(make_single_header PRIVATE cxx_std_23)
target_link_libraries(make_single_header PRIVATE incplot)

if (CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(make_single_header PRIVATE stdc++fs)
endif()
