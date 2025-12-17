#include <gtest/gtest.h>
#include <type_traits>

template <typename T, typename... Ts>
constexpr auto max(T first, Ts... rest) {
  using R = std::common_type_t<T, Ts...>;
  R result = first;
  ((result = result > rest ? result : rest), ...);
  return result;
}

template <typename T, typename... Ts>
constexpr auto min(T first, Ts... rest) {
  using R = std::common_type_t<T, Ts...>;
  R result = first;
  ((result = result < rest ? result : rest), ...);
  return result;
}

template <typename... Ts>
constexpr auto sum(Ts... args) {
  return (args + ...);
}

template <typename... Ts>
constexpr double mean(Ts... args) {
  return static_cast<double>(sum(args...)) / sizeof...(Ts);
}

TEST(test, instantiates_max_functions) {
  EXPECT_EQ(10, max(1, 2, 3, 4, 5, 7, 8, 9, 10));
  EXPECT_EQ(10.0, max(1, 2, 3, 4, 4.9, 7, 8, 9, 10.0));
}

TEST(test, instantiates_min_functions) {
  EXPECT_EQ(1, min(1, 2, 3, 4, 5, 7, 8, 9, 10));
  EXPECT_EQ(1.0, min(1, 2, 3, 4, 4.9, 7, 8, 9, 10.0));
}

TEST(test, instantiates_mean_functions) {
  EXPECT_EQ(5.0, mean(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
  EXPECT_EQ(5.0, mean(0, 1, 2, 3, 4, 5.0, 6, 7, 8, 9, 10.0));
}

TEST(test, instantiates_sum_functions) {
  EXPECT_EQ(49, sum(1, 2, 3, 4, 5, 7, 8, 9, 10));
  EXPECT_EQ(48.9, sum(1, 2, 3, 4, 4.9, 7, 8, 9, 10.0));
}
