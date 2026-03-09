#include <cassert>
#include <compare>
#include <exception>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <variant>
#include <vector>

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

class Exception : public std::exception {
private:
    std::string message;

public:
    explicit Exception(std::string msg) : message(std::move(msg)) {}

    const char* what() const noexcept override {
        return message.c_str();
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
            throw Exception("Rational error: denominator must not be zero");
        }

        if constexpr (std::integral<T>) {
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
        if (denominator == T{}) {
            throw Exception("Rational constructor error: zero denominator");
        }
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
            throw Exception("Rational division error: division by zero rational");
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

void test_rational() {
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

    auto sum = r1 + r3;
    auto diff = r3 - r1;
    auto prod = r1 * r3;
    auto quot = r3 / r1;

    std::cout << "r1 + r3 = ";
    sum.print();
    std::cout << "r3 - r1 = ";
    diff.print();
    std::cout << "r1 * r3 = ";
    prod.print();
    std::cout << "r3 / r1 = ";
    quot.print();

    Rational<int> x(3, 2);
    std::cout << "x: ";
    x.print();

    std::cout << "++x: ";
    (++x).print();
    std::cout << "x++: ";
    (x++).print();
    std::cout << "after x++: ";
    x.print();

    std::cout << "--x: ";
    (--x).print();
    std::cout << "x--: ";
    (x--).print();
    std::cout << "after x--: ";
    x.print();
}

void demonstrate_bad_variant_access() {
    std::variant<int, std::string> value = 42;
    std::cout << "Trying std::get<std::string> from variant holding int...\n";
    [[maybe_unused]] auto s = std::get<std::string>(value);
}

void demonstrate_bad_optional_access() {
    std::optional<int> value;
    std::cout << "Trying value() on empty optional...\n";
    [[maybe_unused]] int x = value.value();
}

void demonstrate_length_error() {
    std::vector<int> v;
    std::cout << "Trying vector::reserve(max_size() + 1)...\n";
    v.reserve(v.max_size() + 1);
}

void demonstrate_out_of_range() {
    std::vector<int> v {1, 2, 3};
    std::cout << "Trying vector::at(10)...\n";
    [[maybe_unused]] int x = v.at(10);
}

void demonstrate_bad_alloc() {
    std::cout << "Trying to allocate extremely large dynamic array...\n";
    std::size_t huge = static_cast<std::size_t>(-1) / sizeof(int);
    int* ptr = new int[huge];
    delete[] ptr;
}

int main() {
    try {
        std::cout << "=== Rational tests ===\n";
        test_rational();

        std::cout << "\n=== Custom exception demonstration ===\n";
        Rational<int> invalid(1, 0);
        invalid.print();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught unknown exception\n";
    }

    try {
        std::cout << "\n=== std::bad_variant_access demonstration ===\n";
        demonstrate_bad_variant_access();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught unknown exception\n";
    }

    try {
        std::cout << "\n=== std::bad_optional_access demonstration ===\n";
        demonstrate_bad_optional_access();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught unknown exception\n";
    }

    try {
        std::cout << "\n=== std::length_error demonstration ===\n";
        demonstrate_length_error();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught unknown exception\n";
    }

    try {
        std::cout << "\n=== std::out_of_range demonstration ===\n";
        demonstrate_out_of_range();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught unknown exception\n";
    }

    try {
        std::cout << "\n=== std::bad_alloc demonstration ===\n";
        demonstrate_bad_alloc();
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << '\n';
    }
    catch (...) {
        std::cerr << "Caught unknown exception\n";
    }

    return 0;
}
