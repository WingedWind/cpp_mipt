#include <gtest/gtest.h>

//////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstddef>
#include <utility>

//////////////////////////////////////////////////////////////////

template <typename... Ts> class Tuple {};

//////////////////////////////////////////////////////////////////

template <typename T, typename... Ts>
class Tuple<T, Ts...> {
public:
  template <typename U, typename... Us>
  constexpr Tuple(U &&x, Us &&...xs)
      : m_head(std::forward<U>(x)),

        m_tail(std::forward<Us>(xs)...) {}

  //  ------------------------------------------------

  template <std::size_t I> constexpr auto get() const {
    if constexpr (I > 0) {
      return m_tail.template get<I - 1>();
    } else {
      return m_head;
    }
  }

  constexpr auto size() const { return sizeof...(Ts) + 1; }

private:
  T m_head;

  Tuple<Ts...> m_tail;
};

//////////////////////////////////////////////////////////////////

TEST(test, tuples) {
  static constexpr auto IntOne = 1;
  static constexpr auto DoubleOne = 1.0;
  static constexpr Tuple<int, double> tuple {IntOne, DoubleOne};

  static_assert(tuple.size() == 2);
  static_assert(tuple.get<0>() == IntOne);
  static_assert(tuple.get<1>() == DoubleOne);
}
