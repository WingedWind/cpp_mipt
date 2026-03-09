#include <cassert>
#include <compare>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <type_traits>

template <typename Derived>
struct addable {
    friend Derived operator+(Derived lhs, Derived const& rhs) {
        lhs += rhs;
        return lhs;
    }
};

template <typename Derived>
struct subtractable {
    friend Derived operator-(Derived lhs, Derived const& rhs) {
        lhs -= rhs;
        return lhs;
    }
};

template <typename Derived>
struct multipliable {
    friend Derived operator*(Derived lhs, Derived const& rhs) {
        lhs *= rhs;
        return lhs;
    }
};

template <typename Derived>
struct dividable {
    friend Derived operator/(Derived lhs, Derived const& rhs) {
        lhs /= rhs;
        return lhs;
    }
};

template <typename Derived>
struct incrementable {
    friend Derived operator++(Derived& value, int) {
        Derived old = value;
        ++value;
        return old;
    }
};

template <typename Derived>
struct decrementable {
    friend Derived operator--(Derived& value, int) {
        Derived old = value;
        --value;
        return old;
    }
};

template <typename T>
class Rational
    : public addable<Rational<T>>
    , public subtractable<Rational<T>>
    , public multipliable<Rational<T>>
    , public dividable<Rational<T>>
    , public incrementable<Rational<T>>
    , public decrementable<Rational<T>> {
private:
    T numerator {};
    T denominator {1};

    void normalize() {
        if (denominator == T{}) {
            throw std::invalid_argument("Denominator must not be zero");
        }

        if constexpr (std::is_integral_v<T>) {
            if (denominator < T{}) {
                numerator = -numerator;
                denominator = -denominator;
            }

            T g = std::gcd(numerator, denominator);
            if (g != T{}) {
                numerator /= g;
                denominator /= g;
            }
        } else {
            if (denominator < T{}) {
                numerator = -numerator;
                denominator = -denominator;
            }
        }
    }

public:
    Rational() = default;

    Rational(T num, T denom) : numerator(num), denominator(denom) {
        normalize();
    }

    T num() const {
        return numerator;
    }

    T den() const {
        return denominator;
    }

    Rational& operator+=(Rational const& other) {
        numerator = numerator * other.denominator + other.numerator * denominator;
        denominator = denominator * other.denominator;
        normalize();
        return *this;
    }

    Rational& operator-=(Rational const& other) {
        numerator = numerator * other.denominator - other.numerator * denominator;
        denominator = denominator * other.denominator;
        normalize();
        return *this;
    }

    Rational& operator*=(Rational const& other) {
        numerator *= other.numerator;
        denominator *= other.denominator;
        normalize();
        return *this;
    }

    Rational& operator/=(Rational const& other) {
        if (other.numerator == T{}) {
            throw std::invalid_argument("Division by zero rational");
        }

        numerator *= other.denominator;
        denominator *= other.numerator;
        normalize();
        return *this;
    }

    Rational& operator++() {
        numerator += denominator;
        normalize();
        return *this;
    }

    Rational& operator--() {
        numerator -= denominator;
        normalize();
        return *this;
    }

    std::strong_ordering operator<=>(Rational const& other) const {
        auto lhs = numerator * other.denominator;
        auto rhs = denominator * other.numerator;

        if (lhs < rhs) {
            return std::strong_ordering::less;
        }
        if (lhs > rhs) {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

    bool operator==(Rational const& other) const {
        return (*this <=> other) == std::strong_ordering::equal;
    }

    void print() const {
        std::cout << numerator << '/' << denominator << '\n';
    }
};

void run_tests() {
    Rational<int> r1(3, 4);
    Rational<int> r2(6, 8);
    Rational<int> r3(5, 4);

    std::cout << "r1: ";
    r1.print();
    std::cout << "r2: ";
    r2.print();
    std::cout << "r3: ";
    r3.print();

    assert(r1 == r2);
    assert(r1 < r3);
    assert(!(r1 > r3));

    auto sum = r1 + r3;      // addable
    auto diff = r3 - r1;     // subtractable
    auto prod = r1 * r3;     // multipliable
    auto quot = r3 / r1;     // dividable

    std::cout << "r1 + r3 = ";
    sum.print();
    std::cout << "r3 - r1 = ";
    diff.print();
    std::cout << "r1 * r3 = ";
    prod.print();
    std::cout << "r3 / r1 = ";
    quot.print();

    Rational<int> x(3, 2);   // 3/2
    std::cout << "x: ";
    x.print();

    std::cout << "++x: ";
    (++x).print();           // prefix increment
    std::cout << "x++: ";
    (x++).print();           // postfix increment from mixin
    std::cout << "after x++: ";
    x.print();

    std::cout << "--x: ";
    (--x).print();           // prefix decrement
    std::cout << "x--: ";
    (x--).print();           // postfix decrement from mixin
    std::cout << "after x--: ";
    x.print();

    Rational<long long> r4(3LL, 4LL);
    Rational<double> r5(0.5, 1.0);

    std::cout << "r4: ";
    r4.print();
    std::cout << "r5: ";
    r5.print();
}

int main() {
    run_tests();
    return 0;
}
