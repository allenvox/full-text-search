set(target_name searcher-c)
add_library(${target_name} SHARED
  ${PROJECT_SOURCE_DIR}/include/${target_name}/${target_name}.h
  searcher-c.cpp
  ${PROJECT_SOURCE_DIR}/include/config/config.hpp
  ${PROJECT_SOURCE_DIR}/src/config/config.cpp
  ${PROJECT_SOURCE_DIR}/include/searcher/searcher.hpp
  ${PROJECT_SOURCE_DIR}/src/searcher/searcher.cpp
)
target_compile_options(${target_name} PRIVATE -fPIC)
set_target_properties(
  ${target_name}
  PROPERTIES
    CXX_STANDARD 17
)
target_include_directories(${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/_deps/picosha2-src
)
target_link_libraries(${target_name}
  PRIVATE
    common
)