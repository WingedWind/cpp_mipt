#include <cstddef>
#include <iostream>

#include <boost/multi_array.hpp>

namespace
{
    constexpr auto size = 10uz;
    using Grid = boost::multi_array<int, 2>;
}

int neighbours(const Grid& grid, std::size_t row, std::size_t column)
{
    auto count = 0;

    for (auto dr = -1; dr <= 1; ++dr)
    {
        for (auto dc = -1; dc <= 1; ++dc)
        {
            if (dr == 0 && dc == 0)
            {
                continue;
            }

            const auto nr = static_cast<int>(row) + dr;
            const auto nc = static_cast<int>(column) + dc;

            if (nr >= 0 && nr < static_cast<int>(size) && nc >= 0 && nc < static_cast<int>(size))
            {
                count += grid[nr][nc];
            }
        }
    }

    return count;
}

void print(const Grid& grid)
{
    for (auto row = 0uz; row < size; ++row)
    {
        for (auto column = 0uz; column < size; ++column)
        {
            std::cout << (grid[row][column] ? '#' : '.');
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}

int main()
{
    Grid current(boost::extents[size][size]);
    Grid next(boost::extents[size][size]);

    current[4][3] = 1;
    current[4][4] = 1;
    current[4][5] = 1;
    current[3][5] = 1;
    current[2][4] = 1;

    for (auto step = 0; step < 8; ++step)
    {
        std::cout << "step " << step << '\n';
        print(current);

        for (auto row = 0uz; row < size; ++row)
        {
            for (auto column = 0uz; column < size; ++column)
            {
                const auto alive = current[row][column] == 1;
                const auto count = neighbours(current, row, column);

                next[row][column] = (alive && (count == 2 || count == 3)) || (!alive && count == 3);
            }
        }

        current = next;
    }
}
