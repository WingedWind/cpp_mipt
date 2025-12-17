module;

export module rational;

import <compare>;
import <iosfwd>;

export namespace math
{
    class Rational
    {
    public:
        Rational(int numerator = 0, int denominator = 1);

        std::strong_ordering operator<=>(const Rational& other) const;
        bool operator==(const Rational& other) const;

        void print() const;

    private:
        void normalize();

        int m_numerator;
        int m_denominator;
    };
}
