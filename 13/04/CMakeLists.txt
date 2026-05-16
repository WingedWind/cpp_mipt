add_executable(directory_grep 13.04.cc)
target_compile_features(directory_grep PUBLIC cxx_std_23)
target_compile_options(directory_grep PRIVATE -Wall -Wextra -Wpedantic)
