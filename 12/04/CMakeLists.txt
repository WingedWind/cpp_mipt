add_executable(email_regex 12.04.cc)
target_compile_features(email_regex PUBLIC cxx_std_23)
target_compile_options(email_regex PRIVATE -Wall -Wextra -Wpedantic)
