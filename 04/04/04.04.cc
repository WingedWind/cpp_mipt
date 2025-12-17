#include <iostream>
#include <compare>
#include <cassert>

template <typename T>
class Rational {
private:
    T numerator;
    T denominator;

public:
    Rational(T num, T denom) : numerator(num), denominator(denom) {}

    std::strong_ordering operator<=>(const Rational& other) const {
        auto lhs = numerator * other.denominator;
        auto rhs = denominator * other.numerator;

        if (lhs < rhs) return std::strong_ordering::less;
        if (lhs > rhs) return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

    bool operator==(const Rational& other) const {
        return (*this <=> other) == std::strong_ordering::equal;
    }

    void print() const {
        std::cout << numerator << '/' << denominator << std::endl;
    }
};

void run_tests() {
    Rational<int> r1(3, 4);     // 3/4
    Rational<int> r2(6, 8);     // 6/8
    Rational<int> r3(5, 4);     // 5/4

    std::cout << "r1: ";
    r1.print();
    std::cout << "r2: ";
    r2.print();
    std::cout << "r3: ";
    r3.print();

    std::cout << "r1 == r2: " << (r1 == r2) << std::endl;
    assert(r1 == r2);
    std::cout << "r1 < r3: " << (r1 < r3) << std::endl;
    assert(r1 < r3);
    std::cout << "r1 > r3: " << (r1 > r3) << std::endl;
    assert(!(r1 > r3));

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

