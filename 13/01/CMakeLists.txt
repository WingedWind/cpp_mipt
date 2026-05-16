add_executable(hex_bytes 13.01.cc)
target_compile_features(hex_bytes PUBLIC cxx_std_23)
target_compile_options(hex_bytes PRIVATE -Wall -Wextra -Wpedantic)
