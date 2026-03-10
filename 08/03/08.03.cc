#include <iostream>
#include <stdexcept>

int intLog2(int x)
{
    unsigned int u = static_cast<unsigned int>(x);

    int result = -1;
    while (u != 0)
    {
        u >>= 1;
        ++result;
    }

    return result;
}

int floatLog2(float x)
{
    union
    {
        float f;
        unsigned int u;
    } value { x };

    unsigned int bits = value.u;
    unsigned int exponent = (bits >> 23) & 0xFFu;
    unsigned int mantissa = bits & 0x7FFFFFu;

    if (exponent == 0xFFu)
    {
        throw std::runtime_error("log2 is undefined for inf and nan");
    }

    if (exponent != 0)
    {
        return static_cast<int>(exponent) - 127;
    }

    if (mantissa == 0)
    {
        throw std::runtime_error("log2 is undefined for zero");
    }

    int k = -1;
    while (mantissa != 0)
    {
        mantissa >>= 1;
        ++k;
    }

    return k - 149;
}

int main()
{
    std::cout << "intLog2(1)   = " << intLog2(1) << '\n';
    std::cout << "intLog2(2)   = " << intLog2(2) << '\n';
    std::cout << "intLog2(3)   = " << intLog2(3) << '\n';
    std::cout << "intLog2(16)  = " << intLog2(16) << '\n';
    std::cout << "intLog2(31)  = " << intLog2(31) << '\n';

    std::cout << "floatLog2(1.0f)  = " << floatLog2(1.0f) << '\n';
    std::cout << "floatLog2(2.0f)  = " << floatLog2(2.0f) << '\n';
    std::cout << "floatLog2(3.5f)  = " << floatLog2(3.5f) << '\n';
    std::cout << "floatLog2(0.75f) = " << floatLog2(0.75f) << '\n';
    std::cout << "floatLog2(0.5f)  = " << floatLog2(0.5f) << '\n';
    std::cout << "floatLog2(0.1f)  = " << floatLog2(0.1f) << '\n';


    return 0;
}

/*
 * Score is 10/10
 */
