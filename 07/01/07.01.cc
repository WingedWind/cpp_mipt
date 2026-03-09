#include <gtest/gtest.h>
#include <optional>
#include <utility>
#include <variant>

using roots_t = std::variant<std::monostate, double, std::pair<double, double>>;

std::optional<roots_t> solve(double a, double b, double c);

#include <cmath>

std::optional<roots_t> solve(double a, double b, double c)
{
    const double epsilon = 1e-12;

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
    const double x1 = (-b - sqrt_d) / (2.0 * a);
    const double x2 = (-b + sqrt_d) / (2.0 * a);

    return roots_t{std::pair<double, double>{x1, x2}};
}

namespace
{
    constexpr double epsilon = 1e-12;
}

TEST(QuadraticEquationTest, NoRootsForNegativeDiscriminant)
{
    const auto result = solve(1.0, 0.0, 1.0);
    ASSERT_FALSE(result.has_value());
}

TEST(QuadraticEquationTest, OneRootForZeroDiscriminant)
{
    const auto result = solve(1.0, 2.0, 1.0);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(std::holds_alternative<double>(*result));
    EXPECT_NEAR(std::get<double>(*result), -1.0, epsilon);
}

TEST(QuadraticEquationTest, TwoRootsForPositiveDiscriminant)
{
    const auto result = solve(1.0, -3.0, 2.0);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE((std::holds_alternative<std::pair<double, double>>(*result)));

    const auto roots = std::get<std::pair<double, double>>(*result);
    EXPECT_NEAR(roots.first, 1.0, epsilon);
    EXPECT_NEAR(roots.second, 2.0, epsilon);
}

TEST(QuadraticEquationTest, LinearEquationHasOneRoot)
{
    const auto result = solve(0.0, 2.0, -4.0);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(std::holds_alternative<double>(*result));
    EXPECT_NEAR(std::get<double>(*result), 2.0, epsilon);
}

TEST(QuadraticEquationTest, LinearEquationHasNoRoots)
{
    const auto result = solve(0.0, 0.0, 5.0);
    ASSERT_FALSE(result.has_value());
}

TEST(QuadraticEquationTest, EquationHasInfiniteNumberOfRoots)
{
    const auto result = solve(0.0, 0.0, 0.0);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(std::holds_alternative<std::monostate>(*result));
}
