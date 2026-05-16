add_executable(calculator_from_file 13.02.cc)
target_compile_features(calculator_from_file PUBLIC cxx_std_23)
target_compile_options(calculator_from_file PRIVATE -Wall -Wextra -Wpedantic)
