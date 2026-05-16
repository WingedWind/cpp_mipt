add_executable(remove_blank_lines 13.03.cc)
target_compile_features(remove_blank_lines PUBLIC cxx_std_23)
target_compile_options(remove_blank_lines PRIVATE -Wall -Wextra -Wpedantic)
