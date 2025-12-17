#include <gtest/gtest.h>
#include <vector>
#include <utility>

template <typename Container>
constexpr void handle(Container& cont, int value) {
  cont.push_back(value);
}

template <typename Container, typename T>
constexpr void handle(Container&, T&&) {
  // intentionally ignored
}

template <typename Container, typename... Args>
constexpr void insert(Container& cont, Args&&... args) {
  (handle(cont, std::forward<Args>(args)), ...);
}

TEST(test, inserts_ints) {
  std::vector<int> vec;
  insert(vec, 1, 2, 3);

  EXPECT_EQ(vec.size(), 3);
}

TEST(test, skips) {
  std::vector<int> vec;
  insert(vec, 1, 2, 3, "1", 1.0, vec);

  EXPECT_EQ(vec.size(), 3);
}
