#include <cassert>
#include <cmath>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

namespace
{
    constexpr int expected_test_count()
    {
        return 4;
    }

    constexpr double epsilon()
    {
        return 1e-12;
    }
}

class Parser
{
public:
    explicit Parser(std::string_view expression)
        : m_expression(expression)
    {
    }

    double parse()
    {
        const auto result = expression();
        skip_spaces();
        return result;
    }

private:
    double expression()
    {
        auto result = term();

        while (true)
        {
            if (consume('+'))
            {
                result += term();
            }
            else if (consume('-'))
            {
                result -= term();
            }
            else
            {
                return result;
            }
        }
    }

    double term()
    {
        auto result = power();

        while (true)
        {
            if (consume('*'))
            {
                result *= power();
            }
            else if (consume('/'))
            {
                result /= power();
            }
            else if (consume('%'))
            {
                result = std::fmod(result, power());
            }
            else
            {
                return result;
            }
        }
    }

    double power()
    {
        auto result = unary();

        if (consume('^'))
        {
            result = std::pow(result, power());
        }

        return result;
    }

    double unary()
    {
        if (consume('+'))
        {
            return unary();
        }

        if (consume('-'))
        {
            return -unary();
        }

        return postfix();
    }

    double postfix()
    {
        auto result = primary();

        while (consume('!'))
        {
            result = factorial(result);
        }

        return result;
    }

    double primary()
    {
        skip_spaces();

        if (consume('('))
        {
            return bracketed(')');
        }

        if (consume('['))
        {
            return bracketed(']');
        }

        if (consume('{'))
        {
            return bracketed('}');
        }

        return number();
    }

    double bracketed(char close)
    {
        const auto result = expression();
        assert(consume(close));
        return result;
    }

    double number()
    {
        skip_spaces();
        const auto begin = m_position;

        while (m_position < m_expression.size() && (std::isdigit(static_cast<unsigned char>(m_expression[m_position])) || m_expression[m_position] == '.'))
        {
            ++m_position;
        }

        return std::stod(std::string(m_expression.substr(begin, m_position - begin)));
    }

    static double factorial(double value)
    {
        auto result = 1.0;

        for (auto factor = 2; factor <= static_cast<int>(value); ++factor)
        {
            result *= factor;
        }

        return result;
    }

    bool consume(char symbol)
    {
        skip_spaces();

        if (m_position < m_expression.size() && m_expression[m_position] == symbol)
        {
            ++m_position;
            return true;
        }

        return false;
    }

    void skip_spaces()
    {
        while (m_position < m_expression.size() && std::isspace(static_cast<unsigned char>(m_expression[m_position])))
        {
            ++m_position;
        }
    }

    std::string_view m_expression;
    std::size_t m_position = 0;
};

double calculate(std::string_view expression)
{
    return Parser(expression).parse();
}

std::filesystem::path create_test_data()
{
    const auto path = std::filesystem::temp_directory_path() / "calculator_from_file_expressions.txt";

    std::fstream file(path, std::ios::out);
    assert(file.is_open());

    file << "2 + 2 = 4\n"
         << "2 ^ 3 ^ 2 = 512\n"
         << "5! + 10 % 4 = 122\n"
         << "[2 + 3] * {4 + 1} = 25\n";

    return path;
}

int main()
{
    const auto path = create_test_data();
    std::fstream file(path, std::ios::in);
    assert(file.is_open());

    std::string line;
    auto checked = 0;

    while (std::getline(file, line))
    {
        const auto separator = line.find('=');
        assert(separator != std::string::npos);

        const auto expression = std::string_view(line).substr(0, separator);
        const auto expected = std::stod(line.substr(separator + 1));

        assert(std::abs(calculate(expression) - expected) < epsilon());
        ++checked;
    }

    assert(checked == expected_test_count());
    std::filesystem::remove(path);
}
