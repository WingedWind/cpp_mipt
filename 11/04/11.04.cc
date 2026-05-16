#include <cassert>
#include <cmath>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

using roots_t = std::variant<std::monostate, double, std::pair<double, double>>;

std::optional<roots_t> solve(double a, double b, double c)
{
    constexpr double epsilon = 1e-12;

    if (std::abs(a) < epsilon)
    {
        if (std::abs(b) < epsilon)
        {
            if (std::abs(c) < epsilon)
            {
                return roots_t{std::monostate{}};
            }

            return std::nullopt;
        }

        return roots_t{-c / b};
    }

    const double d = b * b - 4.0 * a * c;

    if (std::abs(d) < epsilon)
    {
        return roots_t{-b / (2.0 * a)};
    }

    if (d < 0.0)
    {
        return std::nullopt;
    }

    const double sqrt_d = std::sqrt(d);
    return roots_t{std::pair<double, double>{(-b - sqrt_d) / (2.0 * a), (-b + sqrt_d) / (2.0 * a)}};
}

class Visitor
{
public:
    std::vector<double> operator()(std::monostate) const
    {
        return {};
    }

    std::vector<double> operator()(double root) const
    {
        return {root};
    }

    std::vector<double> operator()(const std::pair<double, double>& roots) const
    {
        return {roots.first, roots.second};
    }
};

std::optional<std::vector<double>> roots(double a, double b, double c)
{
    const auto result = solve(a, b, c);

    if (!result.has_value())
    {
        return std::nullopt;
    }

    return std::visit(Visitor(), *result);
}

int main()
{
    constexpr double epsilon = 1e-12;

    assert(!roots(1.0, 0.0, 1.0).has_value());

    auto one_root = roots(1.0, 2.0, 1.0);
    assert(one_root.has_value());
    assert(one_root->size() == 1);
    assert(std::abs(one_root->front() + 1.0) < epsilon);

    auto two_roots = roots(1.0, -3.0, 2.0);
    assert(two_roots.has_value());
    assert(two_roots->size() == 2);
    assert(std::abs((*two_roots)[0] - 1.0) < epsilon);
    assert(std::abs((*two_roots)[1] - 2.0) < epsilon);

    auto infinite_roots = roots(0.0, 0.0, 0.0);
    assert(infinite_roots.has_value());
    assert(infinite_roots->empty());
}

