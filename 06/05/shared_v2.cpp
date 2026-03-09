#include <iostream>
#include <boost/config.hpp>

extern "C" BOOST_SYMBOL_EXPORT void test()
{
    std::cout << "Вызвана функция test из библиотеки shared_v2\n";
}
