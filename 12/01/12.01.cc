#include <cassert>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

long double read_rubles(std::string input)
{
    for (char& symbol : input)
    {
        if (symbol == ',')
        {
            symbol = '.';
        }
    }

    long double rubles = 0.0L;
    std::stringstream stream(input);

    try
    {
        stream.imbue(std::locale("ru_RU.utf8"));
        long double kopecks = 0.0L;
        stream >> std::get_money(kopecks, true);

        if (stream)
        {
            return kopecks / 100.0L;
        }
    }
    catch (const std::runtime_error&)
    {
    }

    stream.clear();
    stream.str(input);
    stream >> rubles;
    return rubles;
}

std::string format_usd(long double dollars)
{
    std::stringstream stream;

    try
    {
        stream.imbue(std::locale("en_US.utf8"));
        stream << std::showbase << std::put_money(dollars * 100.0L, true);
        return stream.str();
    }
    catch (const std::runtime_error&)
    {
        stream << '$' << std::fixed << std::setprecision(2) << static_cast<double>(dollars);
        return stream.str();
    }
}

std::string convert_rub_to_usd(std::string input, long double rate)
{
    return format_usd(read_rubles(input) / rate);
}

int main()
{
    const auto dollars = convert_rub_to_usd("9000,00 RUB", 90.0L);
    assert(dollars.find("100") != std::string::npos);
}

