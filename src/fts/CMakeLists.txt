set(target_name fts)

add_library(${target_name} STATIC
  ${PROJECT_SOURCE_DIR}/include/fts/fts.hpp
  fts.cpp)

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)