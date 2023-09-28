set(target_name indexer)

add_library(${target_name} STATIC
  ${PROJECT_SOURCE_DIR}/include/${target_name}/${target_name}.hpp
  indexer.cpp
)

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/_deps/picosha2-src
)

target_link_libraries(
  ${target_name}
  PRIVATE
    common
)

set(test_name indexer.test)

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
  indexer
)

include(GoogleTest)
gtest_discover_tests(${test_name})