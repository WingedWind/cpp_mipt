#include <algorithm>
#include <cstddef>
#include <random>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

////////////////////////////////////////////////////////////////////////////////////

namespace
{
    constexpr std::size_t insertion_sort_threshold = 16;
    constexpr std::size_t test_vector_size = 1000;
    constexpr int random_bound = 1000;
}

////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void order(std::vector<T>& data, std::size_t left, std::size_t right)
{
    for (std::size_t i = left + 1; i < right; ++i)
    {
        for (std::size_t j = i; j > left; --j)
        {
            if (data[j - 1] > data[j])
            {
                std::swap(data[j], data[j - 1]);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////

template<typename T>
T choosePivot(const std::vector<T>& data, std::size_t left, std::size_t right)
{
    const std::size_t last = right - 1;
    const std::size_t mid = left + (right - left - 1) / 2;

    const auto& first_value = data[left];
    const auto& middle_value = data[mid];
    const auto& last_value = data[last];

    if ((first_value > middle_value) != (first_value > last_value))
    {
        return first_value;
    }
    if ((middle_value > first_value) != (middle_value > last_value))
    {
        return middle_value;
    }
    return last_value;
}

////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void quickSort(std::vector<T>& data, std::size_t left, std::size_t right)
{
    if (right - left <= insertion_sort_threshold)
    {
        order(data, left, right);
        return;
    }

    const auto pivot = choosePivot(data, left, right);

    std::size_t lt = left;
    std::size_t i = left;
    std::size_t gt = right;

    while (i < gt)
    {
        if (data[i] < pivot)
        {
            std::swap(data[lt], data[i]);
            ++lt;
            ++i;
        }
        else if (data[i] > pivot)
        {
            --gt;
            std::swap(data[i], data[gt]);
        }
        else
        {
            ++i;
        }
    }

    if (left < lt)
    {
        quickSort(data, left, lt);
    }
    if (gt < right)
    {
        quickSort(data, gt, right);
    }
}
////////////////////////////////////////////////////////////////////////////////////

template<typename T>
void sort(std::vector<T>& data)
{
    if (data.size() <= 1)
    {
        return;
    }

    quickSort(data, 0, data.size());
}

////////////////////////////////////////////////////////////////////////////////////

TEST(SortTest, DescendingVector)
{
    std::vector<int> data(test_vector_size, 0);
    for (std::size_t i = 0; i < test_vector_size; ++i)
    {
        data[i] = static_cast<int>(test_vector_size - i);
    }

    sort(data);

    EXPECT_TRUE(std::ranges::is_sorted(data));
}

////////////////////////////////////////////////////////////////////////////////////

TEST(SortTest, AlreadySortedVector)
{
    std::vector<int> data(test_vector_size, 0);
    for (std::size_t i = 0; i < test_vector_size; ++i)
    {
        data[i] = static_cast<int>(i);
    }

    sort(data);

    EXPECT_TRUE(std::ranges::is_sorted(data));
}

////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

TEST(SortTest, RandomIntValues)
{
    std::vector<int> data(test_vector_size, 0);

    std::mt19937 engine(42);
    std::uniform_int_distribution<int> distribution(0, random_bound - 1);

    for (std::size_t i = 0; i < test_vector_size; ++i)
    {
        data[i] = distribution(engine);
    }

    sort(data);

    EXPECT_TRUE(std::ranges::is_sorted(data));
}

////////////////////////////////////////////////////////////////////////////////////

TEST(SortTest, RandomDoubleValues)
{
    std::vector<double> data(test_vector_size, 0.0);

    std::mt19937 engine(42);
    std::uniform_real_distribution<double> distribution(0.0, static_cast<double>(random_bound));

    for (std::size_t i = 0; i < test_vector_size; ++i)
    {
        data[i] = distribution(engine);
    }

    sort(data);

    EXPECT_TRUE(std::ranges::is_sorted(data));
}

////////////////////////////////////////////////////////////////////////////////////

TEST(SortTest, BoundaryCases)
{
    std::vector<int> empty;
    sort(empty);
    EXPECT_TRUE(std::ranges::is_sorted(empty));

    std::vector<int> one{42};
    sort(one);
    EXPECT_TRUE(std::ranges::is_sorted(one));

    std::vector<int> small{5, 4, 3, 2, 1};
    sort(small);
    EXPECT_TRUE(std::ranges::is_sorted(small));
}

////////////////////////////////////////////////////////////////////////////////////

TEST(SortTest, ManyEqualElements)
{
    std::vector<int> data(test_vector_size, 7);

    sort(data);

    EXPECT_TRUE(std::ranges::is_sorted(data));
}

////////////////////////////////////////////////////////////////////////////////////

TEST(SortTest, NegativeAndPositiveValues)
{
    std::vector<int> data{5, -1, 3, -10, 0, 8, -3, 2};

    sort(data);

    EXPECT_TRUE(std::ranges::is_sorted(data));
}

////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
