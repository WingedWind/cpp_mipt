find_package(GTest REQUIRED)

add_executable(sort2 07.04.cc)
target_compile_features(sort2 PUBLIC cxx_std_23)

target_link_libraries(sort2 PRIVATE GTest::gtest_main)
