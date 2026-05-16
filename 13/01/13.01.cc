#include <cassert>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::string to_hex(const std::vector<std::uint8_t>& bytes)
{
    constexpr auto byte_width = 2;
    std::stringstream stream;

    for (auto byte : bytes)
    {
        stream << std::hex << std::right << std::setw(byte_width) << std::setfill('0') << static_cast<int>(byte);
    }

    return stream.str();
}

int digit(char symbol)
{
    constexpr auto decimal_digit_count = 10;

    if ('0' <= symbol && symbol <= '9')
    {
        return symbol - '0';
    }

    if ('a' <= symbol && symbol <= 'f')
    {
        return symbol - 'a' + decimal_digit_count;
    }

    throw std::runtime_error("invalid hexadecimal digit");
}

std::vector<std::uint8_t> from_hex(const std::string& text)
{
    constexpr auto digits_per_byte = 2;
    constexpr auto bits_per_nibble = 4;

    if (text.size() % digits_per_byte != 0)
    {
        throw std::runtime_error("hexadecimal string must contain an even number of digits");
    }

    std::vector<std::uint8_t> bytes;
    bytes.reserve(text.size() / digits_per_byte);

    for (auto i = 0uz; i < text.size(); i += digits_per_byte)
    {
        bytes.push_back(static_cast<std::uint8_t>((digit(text[i]) << bits_per_nibble) | digit(text[i + 1])));
    }

    return bytes;
}

int main()
{
    const std::vector<std::uint8_t> bytes = {0x00, 0x0f, 0x10, 0xff};
    assert(to_hex(bytes) == "000f10ff");
    assert(from_hex("000f10ff") == bytes);
}
