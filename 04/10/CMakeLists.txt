find_package(GTest REQUIRED)

add_executable(has 04.10.cc)
target_compile_features(has PUBLIC cxx_std_23)

target_link_libraries(has PRIVATE
    GTest::gtest
    GTest::gtest_main
)