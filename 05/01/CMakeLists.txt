find_package(GTest REQUIRED)

add_executable(builder 05.01.cc)
target_compile_features(builder PUBLIC cxx_std_23)

target_link_libraries(builder PRIVATE
    GTest::gtest
    GTest::gtest_main
)