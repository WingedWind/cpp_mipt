find_package(GTest REQUIRED)

add_executable(quadratic 07.01.cc)
target_compile_features(quadratic PUBLIC cxx_std_23)

target_link_libraries(quadratic PRIVATE GTest::gtest_main)


