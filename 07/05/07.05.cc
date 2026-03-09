#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace
{
    constexpr std::size_t test_vector_size = 10000;
}

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
            else
            {
                break;
            }
        }
    }
}

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

template<typename T>
void quickSort(
    std::vector<T>& data,
    std::size_t left,
    std::size_t right,
    std::size_t insertion_sort_threshold)
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
        quickSort(data, left, lt, insertion_sort_threshold);
    }

    if (gt < right)
    {
        quickSort(data, gt, right, insertion_sort_threshold);
    }
}

template<typename T>
void sort(std::vector<T>& data, std::size_t insertion_sort_threshold)
{
    if (data.size() <= 1)
    {
        return;
    }

    quickSort(data, 0, data.size(), insertion_sort_threshold);
}

std::vector<double> make_test_vector()
{
    std::vector<double> data(test_vector_size);

    for (std::size_t i = 0; i < test_vector_size; ++i)
    {
        data[i] = static_cast<double>(test_vector_size - i);
    }

    return data;
}

static void BM_SortReverseDoubles(benchmark::State& state)
{
    const std::size_t threshold = static_cast<std::size_t>(state.range(0));
    const std::vector<double> source = make_test_vector();

    for (auto _ : state)
    {
        std::vector<double> data = source;

        sort(data, threshold);

        benchmark::DoNotOptimize(data);
        benchmark::ClobberMemory();
    }

    state.SetLabel("threshold=" + std::to_string(threshold));
    state.SetItemsProcessed(
        static_cast<std::int64_t>(state.iterations()) *
        static_cast<std::int64_t>(test_vector_size));
}

BENCHMARK(BM_SortReverseDoubles)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16)
    ->Arg(32)
    ->Arg(64)
    ->Arg(128);

BENCHMARK_MAIN();