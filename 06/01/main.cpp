#include "rational.h"

#include <cassert>
#include <iostream>

static void test_normalization()
{
    const Rational a(1, -2);
    const Rational b(-1, 2);
    const Rational c(-1, -2);

    assert(a == b);
    assert(c > a);
}

static void test_equivalent_fractions()
{
    const Rational a(1, 2);
    const Rational b(2, 4);
    const Rational c(50, 100);

    assert(a == b);
    assert(b == c);
    assert(a == c);
}

static void test_zero_numerator()
{
    const Rational a(0, 1);
    const Rational b(0, 5);
    const Rational c(0, -7);

    assert(a == b);
    assert(b == c);
}

static void test_comparison_with_zero()
{
    const Rational zero(0, 1);
    const Rational positive(1, 3);
    const Rational negative(-1, 3);

    assert(negative < zero);
    assert(zero < positive);
}

static void test_ordering_relations()
{
    const Rational a(1, 4);
    const Rational b(1, 2);
    const Rational c(3, 4);

    assert(a < b);
    assert(b < c);
    assert(a < c);
}

static void test_strict_comparisons()
{
    const Rational a(2, 3);
    const Rational b(3, 5);

    assert(a > b);
    assert(!(a < b));
    assert(!(a == b));
}

static void test_constructor_basic()
{
    const Rational a(5, 7);
    const Rational b(-5, 7);
    const Rational c(5, -7);

    assert(b < a);
    assert(c < Rational(0, 1));
}

static void run_all_tests()
{
    test_normalization();
    test_equivalent_fractions();
    test_zero_numerator();
    test_comparison_with_zero();
    test_ordering_relations();
    test_strict_comparisons();
    test_constructor_basic();
}

static void demo()
{
    const Rational r1(3, 4);
    const Rational r2(5, 6);

    std::cout << "r1 = ";
    r1.print();

    std::cout << "r2 = ";
    r2.print();

    std::cout << "r1 < r2: " << (r1 < r2) << '\n';
}

int main()
{
    run_all_tests();
    demo();

    std::cout << "All tests passed\n";
    return 0;
}
