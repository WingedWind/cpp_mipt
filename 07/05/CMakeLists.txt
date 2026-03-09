find_package(benchmark REQUIRED)

add_executable(sort_benchmark 07.05.cc)
target_compile_features(sort_benchmark PUBLIC cxx_std_23)
target_link_libraries(sort_benchmark PRIVATE benchmark::benchmark benchmark::benchmark_main)
