find_package(GTest REQUIRED)

add_executable(minmax 04.02.cc)
target_compile_features(minmax PUBLIC cxx_std_23)

target_link_libraries(minmax PRIVATE
    GTest::gtest
    GTest::gtest_main
)