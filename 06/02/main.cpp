#include <iostream>
#include <cassert>

import rational;

using math::Rational;

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
}

static void test_zero()
{
    const Rational a(0, 1);
    const Rational b(0, -7);

    assert(a == b);
}

static void run_all_tests()
{
    test_normalization();
    test_equivalent_fractions();
    test_zero();
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
