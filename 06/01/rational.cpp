#include "rational.h"

#include <cassert>
#include <iostream>

Rational::Rational(int num, int den)
    : m_numerator(num),
      m_denominator(den)
{
    assert(m_denominator != 0);
    normalize();
}

std::strong_ordering Rational::operator<=>(const Rational& other) const
{
    const int lhs = m_numerator * other.m_denominator;
    const int rhs = m_denominator * other.m_numerator;

    if (lhs < rhs)
    {
        return std::strong_ordering::less;
    }
    if (lhs > rhs)
    {
        return std::strong_ordering::greater;
    }
    return std::strong_ordering::equal;
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
