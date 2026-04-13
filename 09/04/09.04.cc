#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <iterator>
#include <ranges>
#include <vector>

namespace
{
    constexpr auto insertion_sort_threshold = 16;
    constexpr auto test_vector_size = 1000;
    constexpr auto random_bound = 1000;
}

template<std::random_access_iterator Iterator>
void order(Iterator first, Iterator last)
{
    if (first == last)
    {
        return;
    }

    for (auto iterator = std::next(first); iterator != last; ++iterator)
    {
        for (auto current = iterator; current != first && *current < *std::prev(current); --current)
        {
            std::iter_swap(std::prev(current), current);
        }
    }
}

template<std::random_access_iterator Iterator>
auto choose_pivot(Iterator first, Iterator last)
{
    auto middle = first;
    std::advance(middle, std::distance(first, last) / 2);

    auto tail = std::prev(last);

    const auto& first_value = *first;
    const auto& middle_value = *middle;
    const auto& last_value = *tail;

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

template<std::random_access_iterator Iterator>
void quick_sort(Iterator first, Iterator last)
{
    if (std::distance(first, last) <= 1)
    {
        return;
    }

    if (std::distance(first, last) <= insertion_sort_threshold)
    {
        order(first, last);
        return;
    }

    const auto pivot = choose_pivot(first, last);

    auto left = first;
    auto right = std::prev(last);

    while (true)
    {
        while (*left < pivot)
        {
            ++left;
        }

        while (pivot < *right)
        {
            --right;
        }

        if (!(left < right))
        {
            break;
        }

        std::iter_swap(left, right);
        ++left;
        --right;
    }

    quick_sort(first, std::next(right));
    quick_sort(std::next(right), last);
}

template<std::random_access_iterator Iterator>
void sort(Iterator first, Iterator last)
{
    quick_sort(first, last);
}

void run_tests()
{
    {
        std::vector<int> data(test_vector_size, 0);

        for (auto i = 0uz; i < data.size(); ++i)
        {
            data[i] = static_cast<int>(data.size() - i);
        }

        sort(data.begin(), data.end());
        assert(std::ranges::is_sorted(data));
    }

    {
        std::vector<int> data(test_vector_size, 0);

        for (auto i = 0uz; i < data.size(); ++i)
        {
            data[i] = static_cast<int>(i);
        }

        sort(data.begin(), data.end());
        assert(std::ranges::is_sorted(data));
    }

    {
        std::vector<double> data(test_vector_size, 0.0);

        for (auto& value : data)
        {
            value = static_cast<double>(std::rand() % random_bound);
        }

        sort(data.begin(), data.end());
        assert(std::ranges::is_sorted(data));
    }

    {
        std::deque<int> data = {5, 1, 4, 2, 3, 3, 0};

        sort(data.begin(), data.end());
        assert(std::ranges::is_sorted(data));
    }

    {
        std::vector<int> empty;
        sort(empty.begin(), empty.end());
        assert(std::ranges::is_sorted(empty));

        std::vector<int> one = {42};
        sort(one.begin(), one.end());
        assert(std::ranges::is_sorted(one));
    }
}

int main()
{
    run_tests();
}
