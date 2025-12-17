#pragma once

#include <compare>
#include <iosfwd>

class Rational
{
public:
    Rational(int num = 0, int den = 1);

    std::strong_ordering operator<=>(const Rational& other) const;
    bool operator==(const Rational& other) const;

    void print() const;

private:
    void normalize();

    int m_numerator;
    int m_denominator;
};
