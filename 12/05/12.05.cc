#include <cassert>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <string_view>

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

        if (m_position != m_expression.size())
        {
            throw std::runtime_error("unexpected token");
        }

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

        if (!consume(close))
        {
            throw std::runtime_error("missing closing bracket");
        }

        return result;
    }

    double number()
    {
        skip_spaces();
        auto begin = m_position;

        while (m_position < m_expression.size() && (std::isdigit(static_cast<unsigned char>(m_expression[m_position])) || m_expression[m_position] == '.'))
        {
            ++m_position;
        }

        if (begin == m_position)
        {
            throw std::runtime_error("number expected");
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

int main()
{
    assert(calculate("2 + 3 * 4") == 14.0);
    assert(calculate("2 ^ 3 ^ 2") == 512.0);
    assert(calculate("10 % 4") == 2.0);
    assert(calculate("5!") == 120.0);
    assert(calculate("[2 + 3] * {4 + 1}") == 25.0);
    assert(calculate("-3!") == -6.0);
}

