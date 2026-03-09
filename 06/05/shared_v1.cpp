#include <iostream>
#include <boost/config.hpp>

extern "C" BOOST_SYMBOL_EXPORT void test()
{
    std::cout << "Function test from library shared_v1 was called\n";
}
