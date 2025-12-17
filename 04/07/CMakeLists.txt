find_package(GTest REQUIRED)

add_executable(arith 04.07.cc)
target_compile_features(arith PUBLIC cxx_std_23)

target_link_libraries(arith PRIVATE
    GTest::gtest
    GTest::gtest_main
)