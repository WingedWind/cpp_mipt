add_executable(ranges_algorithms 11.05.cc)
target_compile_features(ranges_algorithms PUBLIC cxx_std_23)
target_compile_options(ranges_algorithms PRIVATE -Wall -Wextra -Wpedantic)
