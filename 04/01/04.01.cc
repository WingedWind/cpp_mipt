#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <utility>
#include <vector>

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

    const auto &first_value = data[left];
    const auto &middle_value = data[mid];
    const auto &last_value = data[last];

    // median-of-three по значению
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

    const auto &pivot = choosePivot(data, left, right);

    std::size_t i = left;
    std::size_t j = right - 1;

    // Hoare partition
    while (true)
    {
        while (data[i] < pivot)
        {
            ++i;
        }

        while (data[j] > pivot)
        {
            --j;
        }

        if (i >= j)
        {
            break;
        }

        std::swap(data[i], data[j]);
        ++i;
        --j;
    }

    if (left < j)
    {
        quickSort(data, left, j + 1);
    }
    if (j + 1 < right)
    {
        quickSort(data, j + 1, right);
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

void run_tests()
{
    {
        // Убывающий массив
        std::vector<int> data(test_vector_size, 0);
        for (std::size_t i = 0; i < test_vector_size; ++i)
        {
            data[i] = static_cast<int>(test_vector_size - i);
        }

        sort(data);
        assert(std::ranges::is_sorted(data));
    }

    {
        // Уже отсортированный массив
        std::vector<int> data(test_vector_size, 0);
        for (std::size_t i = 0; i < test_vector_size; ++i)
        {
            data[i] = static_cast<int>(i);
        }

        sort(data);
        assert(std::ranges::is_sorted(data));
    }

    {
        // Случайные значения в ограниченном диапазоне
        std::vector<int> data(test_vector_size, 0);
        for (std::size_t i = 0; i < test_vector_size; ++i)
        {
            data[i] = std::rand() % random_bound;
        }

        sort(data);
        assert(std::ranges::is_sorted(data));
    }

    {
        // Случайные значения типа double диапазоне
        std::vector<double> data(test_vector_size, 0);
        for (std::size_t i = 0; i < test_vector_size; ++i)
        {
            data[i] = static_cast<double>(std::rand() % random_bound);
        }

        sort(data);
        assert(std::ranges::is_sorted(data));
    }

    {
        // Маленькие массивы (граничные случаи)
        std::vector<int> empty;
        sort(empty);
        assert(std::ranges::is_sorted(empty));

        std::vector<int> one{42};
        sort(one);
        assert(std::ranges::is_sorted(one));

        std::vector<int> small{5, 4, 3, 2, 1};
        sort(small);
        assert(std::ranges::is_sorted(small));
    }
}

////////////////////////////////////////////////////////////////////////////////////

int main()
{
    run_tests();
}
////////////////////////////////////////////////////////////////////////////////////

