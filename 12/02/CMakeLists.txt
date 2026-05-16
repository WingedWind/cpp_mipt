add_executable(quine 12.02.cc)
target_compile_features(quine PUBLIC cxx_std_23)
target_compile_options(quine PRIVATE -Wall -Wextra -Wpedantic)
