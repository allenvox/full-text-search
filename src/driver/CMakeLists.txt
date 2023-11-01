set(target_name driver)
add_library(${target_name} STATIC
  ${PROJECT_SOURCE_DIR}/include/${target_name}/${target_name}.hpp
  driver.cpp
)
set_compile_options(${target_name})
target_include_directories(
  ${target_name}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(
  ${target_name}
  PUBLIC
    rapidcsv
  PRIVATE
    indexer
    searcher
    config
)

set(test_name driver.test)
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
  driver
)

include(GoogleTest)
gtest_discover_tests(${test_name})