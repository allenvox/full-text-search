set(target_name config)
add_library(${target_name} STATIC
        ${PROJECT_SOURCE_DIR}/include/${target_name}/${target_name}.hpp
        config.cpp
)
set_compile_options(${target_name})
target_include_directories(
        ${target_name}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)