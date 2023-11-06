set(target_name fts)
add_library(${target_name} STATIC
  ${PROJECT_SOURCE_DIR}/include/${target_name}/${target_name}.hpp
  fts.cpp
)
set_compile_options(${target_name})
target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

set(test_name fts.test)
add_executable(
  ${test_name}
  ${test_name}.cpp
)
set_compile_options(${test_name})
target_include_directories(
  ${test_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(
  ${test_name}
  GTest::gtest_main
  fts
)

include(GoogleTest)
gtest_discover_tests(${test_name})