#include <cassert>
#include <string>
#include <string_view>
#include <vector>

std::string_view longest_palindrome(std::string_view text)
{
    if (text.empty())
    {
        return {};
    }

    const auto size = text.size();
    std::vector<bool> cache(size * size, false);

    auto is_palindrome = [&](auto row, auto column) -> std::vector<bool>::reference
    {
        return cache[row * size + column];
    };

    auto best_begin = 0uz;
    auto best_length = 1uz;

    for (auto begin = 0uz; begin < size; ++begin)
    {
        is_palindrome(begin, begin) = true;
    }

    for (auto length = 2uz; length <= size; ++length)
    {
        for (auto begin = 0uz; begin + length <= size; ++begin)
        {
            const auto end = begin + length - 1;
            const auto inner_is_palindrome = length == 2 || static_cast<bool>(is_palindrome(begin + 1, end - 1));

            if (text[begin] == text[end] && inner_is_palindrome)
            {
                is_palindrome(begin, end) = true;

                if (length > best_length)
                {
                    best_begin = begin;
                    best_length = length;
                }
            }
        }
    }

    return text.substr(best_begin, best_length);
}

int main()
{
    assert(longest_palindrome("babad") == "bab");
    assert(longest_palindrome("cbbd") == "bb");
    assert(longest_palindrome("abc") == "a");
    assert(longest_palindrome("") == "");
}

