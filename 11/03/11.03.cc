#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iterator>
#include <ranges>
#include <vector>

namespace
{
    constexpr auto insertion_sort_threshold = 16;
    constexpr auto test_vector_size = 1000;
    constexpr auto random_bound = 1000;
}

template<typename Comparator, typename T>
bool comes_before(const T& lhs, const T& rhs, Comparator compare)
{
    return std::invoke(compare, lhs, rhs);
}

template<std::random_access_iterator Iterator, typename Comparator>
void order(Iterator first, Iterator last, Comparator compare)
{
    if (first == last)
    {
        return;
    }

    for (auto iterator = std::next(first); iterator != last; ++iterator)
    {
        for (auto current = iterator;
             current != first && comes_before(*current, *std::prev(current), compare);
             --current)
        {
            std::iter_swap(std::prev(current), current);
        }
    }
}

template<std::random_access_iterator Iterator, typename Comparator>
auto choose_pivot(Iterator first, Iterator last, Comparator compare)
{
    auto middle = first;
    std::advance(middle, std::distance(first, last) / 2);

    auto tail = std::prev(last);

    const auto& first_value = *first;
    const auto& middle_value = *middle;
    const auto& last_value = *tail;

    if (comes_before(first_value, middle_value, compare) != comes_before(first_value, last_value, compare))
    {
        return first_value;
    }

    if (comes_before(middle_value, first_value, compare) != comes_before(middle_value, last_value, compare))
    {
        return middle_value;
    }

    return last_value;
}

template<std::random_access_iterator Iterator, typename Comparator>
void quick_sort(Iterator first, Iterator last, Comparator compare)
{
    if (std::distance(first, last) <= 1)
    {
        return;
    }

    if (std::distance(first, last) <= insertion_sort_threshold)
    {
        order(first, last, compare);
        return;
    }

    const auto pivot = choose_pivot(first, last, compare);

    auto left = first;
    auto right = std::prev(last);

    while (true)
    {
        while (comes_before(*left, pivot, compare))
        {
            ++left;
        }

        while (comes_before(pivot, *right, compare))
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

    quick_sort(first, std::next(right), compare);
    quick_sort(std::next(right), last, compare);
}

template<std::random_access_iterator Iterator, typename Comparator = std::less<>>
void sort(Iterator first, Iterator last, Comparator compare = Comparator())
{
    quick_sort(first, last, compare);
}

bool ascending(int lhs, int rhs)
{
    return lhs < rhs;
}

void run_tests()
{
    {
        std::vector<int> data(test_vector_size, 0);

        for (auto i = 0uz; i < data.size(); ++i)
        {
            data[i] = static_cast<int>(data.size() - i);
        }

        sort(data.begin(), data.end(), ascending);
        assert(std::ranges::is_sorted(data, ascending));
    }

    {
        std::vector<int> data(test_vector_size, 0);

        for (auto i = 0uz; i < data.size(); ++i)
        {
            data[i] = static_cast<int>(i);
        }

        sort(data.begin(), data.end(), std::less<>());
        assert(std::ranges::is_sorted(data, std::less<>()));
    }

    {
        std::vector<double> data(test_vector_size, 0.0);

        for (auto& value : data)
        {
            value = static_cast<double>(std::rand() % random_bound);
        }

        auto lambda = [](double lhs, double rhs)
        {
            return lhs > rhs;
        };

        sort(data.begin(), data.end(), lambda);
        assert(std::ranges::is_sorted(data, lambda));
    }

    {
        std::deque<int> data = {5, 1, 4, 2, 3, 3, 0};
        auto lambda = [](int lhs, int rhs)
        {
            return lhs < rhs;
        };

        sort(data.begin(), data.end(), lambda);
        assert(std::ranges::is_sorted(data, lambda));
    }

    {
        std::vector<int> empty;
        sort(empty.begin(), empty.end(), ascending);
        assert(std::ranges::is_sorted(empty, ascending));

        std::vector<int> one = {42};
        sort(one.begin(), one.end(), std::less<>());
        assert(std::ranges::is_sorted(one, std::less<>()));
    }
}

int main()
{
    run_tests();
}
