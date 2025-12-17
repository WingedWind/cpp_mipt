#include <gtest/gtest.h>

////////////////////////////////////////////////////////////////////////////////////

template <int N> struct Fibonachi {
  static_assert(Fibonachi<N - 1>::value <
                std::numeric_limits<int>::max() - Fibonachi<N - 2>::value);
  static constexpr int value =
      Fibonachi<N - 1>::value + Fibonachi<N - 2>::value;
};

template <> struct Fibonachi<0> {
  static constexpr int value = 0;
};

template <> struct Fibonachi<1> {
  static constexpr int value = 1;
};

template <int N> inline constexpr int fibonachi = Fibonachi<N>::value;
////////////////////////////////////////////////////////////////////////////////////

TEST(test, fibonachi_of_0)  { static_assert(fibonachi<0> == 0); }

TEST(test, fibonachi_of_1)  { static_assert(fibonachi<1> == 1); }

TEST(test, fibonachi_of_5)  { static_assert(fibonachi<5> == 5); }

TEST(test, fibonachi_of_10) { static_assert(fibonachi<10> == 55); }
