module;

#include <cassert>

module rational;

import <iostream>;

namespace math
{
    Rational::Rational(int numerator, int denominator)
        : m_numerator(numerator),
          m_denominator(denominator)
    {
        assert(m_denominator != 0);
        normalize();
    }

    std::strong_ordering Rational::operator<=>(const Rational& other) const
    {
        const int lhs = m_numerator * other.m_denominator;
        const int rhs = m_denominator * other.m_numerator;
        return lhs <=> rhs;
    }

    bool Rational::operator==(const Rational& other) const
    {
        return (*this <=> other) == std::strong_ordering::equal;
    }

    void Rational::print() const
    {
        std::cout << m_numerator << '/' << m_denominator << '\n';
    }

    void Rational::normalize()
    {
        if (m_denominator < 0)
        {
            m_numerator = -m_numerator;
            m_denominator = -m_denominator;
        }
    }
}
