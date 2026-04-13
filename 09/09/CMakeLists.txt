find_package(benchmark REQUIRED)

add_executable(fit_allocator_benchmark 09.09.cc)
target_compile_features(fit_allocator_benchmark PUBLIC cxx_std_23)
target_link_libraries(fit_allocator_benchmark PRIVATE benchmark::benchmark)
