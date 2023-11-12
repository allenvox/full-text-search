set(target_name searcher-c)
add_library(${target_name} SHARED
        ${PROJECT_SOURCE_DIR}/include/${target_name}/${target_name}.h
        searcher-c.cpp
)
set_target_properties(
        ${target_name}
        PROPERTIES
        CXX_STANDARD 17
)
target_include_directories(
        ${target_name}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(
        ${target_name}
        PUBLIC
        searcher
        indexer
)