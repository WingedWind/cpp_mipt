add_executable(calculator_extended 12.05.cc)
target_compile_features(calculator_extended PUBLIC cxx_std_23)
target_compile_options(calculator_extended PRIVATE -Wall -Wextra -Wpedantic)
