#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include <boost/graph/adjacency_matrix.hpp>

namespace
{
    using graph_t = boost::adjacency_matrix<boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int>>;
    using path_t = std::vector<int>;

    constexpr int vertex_count()
    {
        return 10;
    }

    constexpr int minimum_edge_weight()
    {
        return 1;
    }

    constexpr int maximum_edge_weight()
    {
        return 10;
    }

    int path_cost(const graph_t& graph, const path_t& path)
    {
        const auto weights = get(boost::edge_weight, graph);
        auto cost = 0;

        for (auto i = 0uz; i + 1 < path.size(); ++i)
        {
            const auto [edge, exists] = boost::edge(path[i], path[i + 1], graph);
            assert(exists);
            cost += get(weights, edge);
        }

        const auto [edge, exists] = boost::edge(path.back(), path.front(), graph);
        assert(exists);
        return cost + get(weights, edge);
    }

    std::pair<path_t, int> solve_tsp(const graph_t& graph)
    {
        path_t path(vertex_count() - 1);
        std::iota(path.begin(), path.end(), 1);

        auto best_path = path;
        auto best_cost = std::numeric_limits<int>::max();

        do
        {
            path_t full_path = {0};
            full_path.insert(full_path.end(), path.begin(), path.end());

            const auto cost = path_cost(graph, full_path);

            if (cost < best_cost)
            {
                best_cost = cost;
                best_path = full_path;
            }
        }
        while (std::next_permutation(path.begin(), path.end()));

        return {best_path, best_cost};
    }
}

int main()
{
    graph_t graph(vertex_count());

    std::random_device device;
    std::default_random_engine generator(device());
    std::uniform_int_distribution<int> distribution(minimum_edge_weight(), maximum_edge_weight());

    for (auto from = 0; from < vertex_count(); ++from)
    {
        for (auto to = from + 1; to < vertex_count(); ++to)
        {
            const auto weight = distribution(generator);
            const auto [edge, inserted] = boost::add_edge(from, to, graph);
            assert(inserted);
            put(boost::edge_weight, graph, edge, weight);
        }
    }

    const auto [best_path, best_cost] = solve_tsp(graph);
    assert(best_path.size() == vertex_count());
    assert(best_cost > 0);

    const auto weights = get(boost::edge_weight, graph);
    std::cout << "matrix:\n";
    for (auto row = 0; row < vertex_count(); ++row)
    {
        for (auto column = 0; column < vertex_count(); ++column)
        {
            if (row == column)
            {
                std::cout << 0 << ' ';
                continue;
            }

            const auto [edge, exists] = boost::edge(row, column, graph);
            std::cout << (exists ? get(weights, edge) : 0) << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "path: ";
    for (auto vertex : best_path)
    {
        std::cout << vertex << ' ';
    }
    std::cout << best_path.front() << "\ncost: " << best_cost << '\n';
}
