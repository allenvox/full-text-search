set(target_name parser)
add_executable(${target_name})
set_compile_options(${target_name})
target_sources(
  ${target_name}
  PRIVATE
    main.cpp
)
target_link_libraries(
  ${target_name}
  PRIVATE
    common
)