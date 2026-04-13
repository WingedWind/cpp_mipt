#include <cassert>
#include <iostream>

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>

using Matrix = boost::numeric::ublas::matrix<unsigned long long int>;

Matrix multiply(const Matrix& lhs, const Matrix& rhs)
{
    Matrix result(2, 2);

    for (auto i = 0uz; i < 2; ++i)
    {
        for (auto j = 0uz; j < 2; ++j)
        {
            result(i, j) = 0;

            for (auto k = 0uz; k < 2; ++k)
            {
                result(i, j) += lhs(i, k) * rhs(k, j);
            }
        }
    }

    return result;
}

Matrix power(Matrix base, unsigned long long exponent)
{
    Matrix result(2, 2);
    result(0, 0) = 1;
    result(0, 1) = 0;
    result(1, 0) = 0;
    result(1, 1) = 1;

    while (exponent > 0)
    {
        if (exponent & 1ULL)
        {
            result = multiply(result, base);
        }

        base = multiply(base, base);
        exponent >>= 1ULL;
    }

    return result;
}

unsigned long long fibonacci(unsigned long long n)
{
    if (n == 0)
    {
        return 0;
    }

    Matrix transform(2, 2);
    transform(0, 0) = 1;
    transform(0, 1) = 1;
    transform(1, 0) = 1;
    transform(1, 1) = 0;

    const auto powered = power(transform, n - 1);
    return powered(0, 0);
}

int main()
{
    assert(fibonacci(0) == 0);
    assert(fibonacci(1) == 1);
    assert(fibonacci(10) == 55);
    assert(fibonacci(20) == 6765);

    std::cout << "F(50) = " << fibonacci(50) << '\n';
    std::cout << "Matrix exponentiation complexity: O(log N)\n";
    std::cout << "Iterative Fibonacci complexity: O(N), naive recursion: O(2^N)\n";
}
