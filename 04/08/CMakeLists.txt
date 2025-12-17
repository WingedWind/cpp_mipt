find_package(GTest REQUIRED)

add_executable(tuple 04.08.cc)
target_compile_features(tuple PUBLIC cxx_std_23)

target_link_libraries(tuple PRIVATE
    GTest::gtest
    GTest::gtest_main
)