find_package(GTest REQUIRED)

add_executable(std-like 04.09.cc)
target_compile_features(std-like PUBLIC cxx_std_23)

target_link_libraries(std-like PRIVATE
    GTest::gtest
    GTest::gtest_main
)