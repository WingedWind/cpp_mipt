#include <cassert>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <iostream>
#include <vector>

void inspect_vector()
{
    std::vector<int> values;
    std::size_t previous = values.capacity();

    std::cout << "vector capacity growth:\n";

    for (auto i = 0; i < 128; ++i)
    {
        values.push_back(i);

        if (values.capacity() != previous)
        {
            const auto old_capacity = previous == 0 ? 1.0 : static_cast<double>(previous);
            const auto factor = static_cast<double>(values.capacity()) / old_capacity;

            std::cout
                << "size=" << std::setw(3) << values.size()
                << " capacity=" << std::setw(3) << values.capacity()
                << " factor=" << std::fixed << std::setprecision(2) << factor << '\n';

            previous = values.capacity();
        }
    }

    assert(values.capacity() >= values.size());
}

void inspect_deque()
{
    std::deque<int> values;
    std::vector<std::uintptr_t> addresses;

    for (auto i = 0; i < 1024; ++i)
    {
        values.push_back(i);
        addresses.push_back(reinterpret_cast<std::uintptr_t>(&values.back()));
    }

    std::vector<std::size_t> page_sizes;
    std::size_t current_page = 1;

    for (auto i = 1uz; i < addresses.size(); ++i)
    {
        if (addresses[i] - addresses[i - 1] == sizeof(int))
        {
            ++current_page;
        }
        else
        {
            page_sizes.push_back(current_page);
            current_page = 1;
        }
    }

    page_sizes.push_back(current_page);

    std::cout << "deque page sizes:\n";
    for (auto i = 0uz; i < page_sizes.size() && i < 5; ++i)
    {
        std::cout << "page " << i << ": " << page_sizes[i] << " elements\n";
    }

    assert(!page_sizes.empty());
}

int main()
{
    inspect_vector();
    inspect_deque();
}
