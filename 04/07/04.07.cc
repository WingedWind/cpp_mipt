#include <gtest/gtest.h>

//////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <numeric>

//////////////////////////////////////////////////////////////////////////////////////////

template <int N = 0, int D = 1> struct Ratio {

  static_assert(D != 0);

  constexpr static auto num = N;

  constexpr static auto den = D;
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> struct Sum {
  constexpr static auto num = R1::num * R2::den + R2::num * R1::den;

  constexpr static auto den = R1::den * R2::den;

  constexpr static auto gcd = std::gcd(num, den);

  //  ------------------------------------------------------------------

  using type = Ratio<gcd == 0 ? 0 : num / gcd, gcd == 0 ? 1 : den / gcd>;
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> using sum = typename Sum<R1, R2>::type;

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> struct Sub {

  using neg_R2 = Ratio<-R2::num, R2::den>;

  //  ------------------------------------------------------------------

  using type = Ratio<sum<R1, neg_R2>::num, sum<R1, neg_R2>::den>;
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> using sub = typename Sub<R1, R2>::type;

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> struct Mul {
  constexpr static auto num = R1::num * R2::num;

  constexpr static auto den = R1::den * R2::den;

  constexpr static auto gcd = std::gcd(num, den);

  using type = Ratio<gcd == 0 ? 0 : num / gcd, gcd == 0 ? 1 : den / gcd>;
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> using mul = typename Mul<R1, R2>::type;

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> struct Div {
  static_assert(R2::num != 0);

  using rev_R2 = Ratio<R2::den, R2::num>;

  //  ------------------------------------------------------------------

  using type = Ratio<mul<R1, rev_R2>::num, mul<R1, rev_R2>::den>;
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename R1, typename R2> using div_type = typename Div<R1, R2>::type;

//////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename R = Ratio<1>> struct Duration {
  T x = T();
};

//////////////////////////////////////////////////////////////////////////////////////////

template <typename T1, typename R1,

          typename T2, typename R2>
constexpr auto operator+(Duration<T1, R1> const &lhs,
                         Duration<T2, R2> const &rhs) {
  using ratio_t = Ratio<1, sum<R1, R2>::den>;

  auto x = (lhs.x * ratio_t::den / R1::den * R1::num +

            rhs.x * ratio_t::den / R2::den * R2::num);

  return Duration<decltype(x), ratio_t>(x);
}

//////////////////////////////////////////////////////////////////////////////////////////

template <typename T1, typename R1,

          typename T2, typename R2>
constexpr auto operator-(Duration<T1, R1> const &lhs,
                         Duration<T2, R2> const &rhs) {

  return lhs + Duration<T2, R2>(-rhs.x);
}

//////////////////////////////////////////////////////////////////////////////////////////

TEST(test, performs_operations_with_ratios) {
  static_assert(std::is_same_v<sum<Ratio<1, 2>, Ratio<2, 3>>, Ratio<7, 6>>);

  static_assert(std::is_same_v<sub<Ratio<1, 2>, Ratio<1, 3>>, Ratio<1, 6>>);
  static_assert(std::is_same_v<sub<Ratio<1, 3>, Ratio<1, 2>>, Ratio<-1, 6>>);
  static_assert(std::is_same_v<sub<Ratio<1, 2>, Ratio<1, 2>>, Ratio<0, 1>>);

  static_assert(std::is_same_v<mul<Ratio<1, 2>, Ratio<2, 3>>, Ratio<1, 3>>);
  static_assert(std::is_same_v<mul<Ratio<-1, 2>, Ratio<2, 3>>, Ratio<-1, 3>>);
  static_assert(std::is_same_v<mul<Ratio<1, 2>, Ratio<0, 14>>, Ratio<0, 1>>);

  static_assert(
      std::is_same_v<div_type<Ratio<1, 2>, Ratio<2, 3>>, Ratio<3, 4>>);
  static_assert(
      std::is_same_v<div_type<Ratio<2, 3>, Ratio<1, 2>>, Ratio<4, 3>>);
}

TEST(test, adds_and_subs_durations) {
  constexpr auto dur_1 = Duration<int, Ratio<1, 12>>(3);
  constexpr auto dur_2 = Duration<int, Ratio<1, 3>>(2);

  constexpr auto dur_sum = dur_1 + dur_2;

  static_assert(dur_sum.x == 11);
  static_assert(std::is_same_v<std::decay_t<decltype(dur_sum)>,
                               Duration<int, Ratio<1, 12>>>);

  constexpr auto dur_sub = dur_1 - dur_2;

  static_assert(dur_sub.x == -5);
  static_assert(std::is_same_v<std::decay_t<decltype(dur_sub)>,
                               Duration<int, Ratio<1, 12>>>);
}
