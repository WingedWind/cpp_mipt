#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

template<std::ranges::input_range Range, typename Predicate, typename Operation>
auto transform_if(Range&& range, Predicate predicate, Operation operation)
{
    using value_t = std::invoke_result_t<Operation, std::ranges::range_reference_t<Range>>;

    std::vector<std::ranges::range_value_t<Range>> filtered;
    std::ranges::copy_if(range, std::back_inserter(filtered), predicate);

    std::vector<value_t> result;
    result.reserve(filtered.size());
    std::ranges::transform(filtered, std::back_inserter(result), operation);
    return result;
}

double mae(const std::vector<double>& expected, const std::vector<double>& actual)
{
    assert(expected.size() == actual.size());

    std::vector<double> errors(expected.size());
    std::ranges::transform(expected, actual, errors.begin(), [](double lhs, double rhs)
    {
        return std::abs(lhs - rhs);
    });

    return std::accumulate(errors.begin(), errors.end(), 0.0) / static_cast<double>(errors.size());
}

double mse(const std::vector<double>& expected, const std::vector<double>& actual)
{
    assert(expected.size() == actual.size());

    std::vector<double> errors(expected.size());
    std::ranges::transform(expected, actual, errors.begin(), [](double lhs, double rhs)
    {
        const double error = lhs - rhs;
        return error * error;
    });

    return std::accumulate(errors.begin(), errors.end(), 0.0) / static_cast<double>(errors.size());
}

class Fibonacci : public std::ranges::view_interface<Fibonacci>
{
public:
    explicit Fibonacci(int count)
        : m_count(count)
    {
    }

    auto begin() const
    {
        return Iterator(0, 0, 1);
    }

    auto end() const
    {
        return Iterator(m_count, 0, 1);
    }

private:
    class Iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = int;

        Iterator() = default;

        Iterator(int index, int previous, int current)
            : m_index(index)
            , m_previous(previous)
            , m_current(current)
        {
            for (auto i = 0; i < index; ++i)
            {
                increment();
            }
        }

        int operator*() const
        {
            return m_previous;
        }

        Iterator& operator++()
        {
            ++m_index;
            increment();
            return *this;
        }

        Iterator operator++(int)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.m_index == rhs.m_index;
        }

    private:
        void increment()
        {
            const int next = m_previous + m_current;
            m_previous = m_current;
            m_current = next;
        }

        int m_index = 0;
        int m_previous = 0;
        int m_current = 1;
    };

    int m_count = 0;
};

int main()
{
    std::vector<int> values = {1, 2, 3, 3, 4, 5};

    std::ranges::replace(values, 3, 9);
    assert((values == std::vector<int>{1, 2, 9, 9, 4, 5}));

    std::ranges::fill(values, 7);
    assert(std::ranges::all_of(values, [](int value) { return value == 7; }));

    values = {1, 1, 2, 2, 3, 3};
    const auto [first, last] = std::ranges::unique(values);
    values.erase(first, last);
    assert((values == std::vector<int>{1, 2, 3}));

    std::ranges::rotate(values, std::next(values.begin()));
    assert((values == std::vector<int>{2, 3, 1}));

    std::vector<int> sampled;
    std::mt19937 generator(42);
    std::ranges::sample(values, std::back_inserter(sampled), 2, generator);
    assert(sampled.size() == 2);

    const auto transformed = transform_if(values, [](int value) { return value > 1; }, [](int value) { return value * value; });
    assert((transformed == std::vector<int>{4, 9}));

    assert(std::abs(mae({1.0, 2.0, 3.0}, {2.0, 2.0, 4.0}) - 2.0 / 3.0) < 1e-12);
    assert(std::abs(mse({1.0, 2.0, 3.0}, {2.0, 2.0, 4.0}) - 2.0 / 3.0) < 1e-12);

    auto filtered = values | std::views::filter([](int value) { return value % 2 == 1; });
    assert((std::vector<int>(filtered.begin(), filtered.end()) == std::vector<int>{3, 1}));

    auto dropped = values | std::views::drop(1);
    assert((std::vector<int>(dropped.begin(), dropped.end()) == std::vector<int>{3, 1}));

    std::vector<std::vector<int>> nested = {{1, 2}, {3}, {4, 5}};
    auto joined = nested | std::views::join;
    assert((std::vector<int>(joined.begin(), joined.end()) == std::vector<int>{1, 2, 3, 4, 5}));

#ifdef __cpp_lib_ranges_zip
    for ([[maybe_unused]] auto pair : std::views::zip(values, values))
    {
        break;
    }
#endif

#ifdef __cpp_lib_ranges_stride
    for ([[maybe_unused]] auto value : values | std::views::stride(2))
    {
        break;
    }
#endif

    const auto fibonacci = Fibonacci(8);
    assert((std::vector<int>(fibonacci.begin(), fibonacci.end()) == std::vector<int>{0, 1, 1, 2, 3, 5, 8, 13}));
}
