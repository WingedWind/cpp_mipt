add_executable(money_locale 12.01.cc)
target_compile_features(money_locale PUBLIC cxx_std_23)
target_compile_options(money_locale PRIVATE -Wall -Wextra -Wpedantic)
