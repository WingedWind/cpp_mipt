find_package(GTest REQUIRED)

add_executable(insert 04.03.cc)
target_compile_features(insert PUBLIC cxx_std_23)

target_link_libraries(insert PRIVATE
    GTest::gtest
    GTest::gtest_main
)