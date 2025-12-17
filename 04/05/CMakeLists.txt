find_package(GTest REQUIRED)

add_executable(fib 04.05.cc)
target_compile_features(fib PUBLIC cxx_std_23)

target_link_libraries(fib PRIVATE
    GTest::gtest
    GTest::gtest_main
)