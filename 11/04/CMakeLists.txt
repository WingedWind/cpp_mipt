add_executable(quadratic_visitor 11.04.cc)
target_compile_features(quadratic_visitor PUBLIC cxx_std_23)
target_compile_options(quadratic_visitor PRIVATE -Wall -Wextra -Wpedantic)
