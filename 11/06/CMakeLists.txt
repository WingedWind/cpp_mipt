add_executable(travelling_salesman 11.06.cc)
target_compile_features(travelling_salesman PUBLIC cxx_std_23)
target_compile_options(travelling_salesman PRIVATE -Wall -Wextra -Wpedantic)
