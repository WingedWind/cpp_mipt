#include <gtest/gtest.h>
#include <type_traits>

namespace MyStd {

//////////////////////////////////////////////////////////////
// is_class
//////////////////////////////////////////////////////////////

template <class T>
std::true_type test_class(int T::*);

template <class>
std::false_type test_class(...);

template <class T>
struct is_class : decltype(test_class<T>(nullptr)) {};

template <class T>
inline constexpr bool is_class_v = is_class<T>::value;

//////////////////////////////////////////////////////////////
// add_const
//////////////////////////////////////////////////////////////

template <class T>
struct add_const {
  using type = const T;
};

template <class T>
using add_const_t = typename add_const<T>::type;

//////////////////////////////////////////////////////////////
// remove_const
//////////////////////////////////////////////////////////////

template <class T>
struct remove_const {
  using type = T;
};

template <class T>
struct remove_const<const T> {
  using type = T;
};

template <class T>
using remove_const_t = typename remove_const<T>::type;

//////////////////////////////////////////////////////////////
// conditional
//////////////////////////////////////////////////////////////

template <bool B, class T, class F>
struct conditional {
  using type = T;
};

template <class T, class F>
struct conditional<false, T, F> {
  using type = F;
};

template <bool B, class T, class F>
using conditional_t = typename conditional<B, T, F>::type;

//////////////////////////////////////////////////////////////
// remove_reference
//////////////////////////////////////////////////////////////

template <class T>
struct remove_reference {
  using type = T;
};

template <class T>
struct remove_reference<T&> {
  using type = T;
};

template <class T>
struct remove_reference<T&&> {
  using type = T;
};

template <class T>
using remove_reference_t = typename remove_reference<T>::type;

//////////////////////////////////////////////////////////////
// is_array
//////////////////////////////////////////////////////////////

template <class T>
struct is_array : std::false_type {};

template <class T>
struct is_array<T[]> : std::true_type {};

template <class T, std::size_t N>
struct is_array<T[N]> : std::true_type {};

template <class T>
inline constexpr bool is_array_v = is_array<T>::value;

//////////////////////////////////////////////////////////////
// remove_extent
//////////////////////////////////////////////////////////////

template <class T>
struct remove_extent {
  using type = T;
};

template <class T>
struct remove_extent<T[]> {
  using type = T;
};

template <class T, std::size_t N>
struct remove_extent<T[N]> {
  using type = T;
};

template <class T>
using remove_extent_t = typename remove_extent<T>::type;

//////////////////////////////////////////////////////////////
// add_pointer
//////////////////////////////////////////////////////////////

template <class T>
struct type_identity {
  using type = T;
};

template <class T>
auto try_add_pointer(int) -> type_identity<remove_reference_t<T>*>;

template <class T>
auto try_add_pointer(...) -> type_identity<T>;

template <class T>
struct add_pointer : decltype(try_add_pointer<T>(0)) {};

template <class T>
using add_pointer_t = typename add_pointer<T>::type;

//////////////////////////////////////////////////////////////
// is_function
//////////////////////////////////////////////////////////////

template <class T>
struct is_function
    : std::integral_constant<
          bool,
          !std::is_const<const T>::value &&
              !std::is_reference<T>::value> {};

template <class T>
inline constexpr bool is_function_v = is_function<T>::value;

//////////////////////////////////////////////////////////////
// decay
//////////////////////////////////////////////////////////////

template <class T>
struct decay {
private:
  using U = remove_reference_t<T>;

public:
  using type =
      conditional_t<
          is_array_v<U>,
          add_pointer_t<remove_extent_t<U>>,
          conditional_t<
              is_function_v<U>,
              add_pointer_t<U>,
              remove_const_t<U>>>;
};

template <class T>
using decay_t = typename decay<T>::type;

//////////////////////////////////////////////////////////////

} // namespace MyStd

auto some_function(int num) { return num; }

TEST(test, tests) {
  class B {};

  static_assert(MyStd::is_class_v<struct A>);
  static_assert(MyStd::is_class_v<class B>);
  static_assert(!MyStd::is_class_v<int>);

  const int n = 0;
  const B some_class;
  const B& some_class_ref = some_class;
  const B array_of_class[10];

  int (*func_ptr[10])(int);

  static_assert(std::is_same_v<decltype(n), const int>);
  static_assert(std::is_same_v<MyStd::decay_t<decltype(n)>, int>);

  static_assert(std::is_same_v<decltype(some_class), const B>);
  static_assert(std::is_same_v<MyStd::decay_t<decltype(some_class)>, B>);

  static_assert(std::is_same_v<decltype(some_class_ref), const B&>);
  static_assert(
      std::is_same_v<MyStd::decay_t<decltype(some_class_ref)>, B>);

  static_assert(std::is_same_v<decltype(array_of_class), const B[10]>);
  static_assert(
      std::is_same_v<MyStd::decay_t<decltype(array_of_class)>, const B*>);

  static_assert(std::is_same_v<decltype(func_ptr), int (*[10])(int)>);
  static_assert(
      std::is_same_v<MyStd::decay_t<decltype(func_ptr)>, int (**)(int)>);
}

