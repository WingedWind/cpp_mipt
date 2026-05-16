#include <cassert>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using email_t = std::pair<std::string, std::string>;

std::vector<email_t> extract_emails(std::string_view text)
{
    const std::regex pattern(R"(([[:alnum:]_.+-]+@([[:alnum:]-]+(?:\.[[:alnum:]-]+)+)))");
    const std::string source(text);

    std::vector<email_t> emails;

    for (std::sregex_iterator iterator(source.begin(), source.end(), pattern), end; iterator != end; ++iterator)
    {
        emails.emplace_back((*iterator)[1].str(), (*iterator)[2].str());
    }

    return emails;
}

int main()
{
    const auto emails = extract_emails(R"(write to first.name@mipt.ru or support+cpp@example.org)");

    assert(emails.size() == 2);
    assert(emails[0].first == "first.name@mipt.ru");
    assert(emails[0].second == "mipt.ru");
    assert(emails[1].first == "support+cpp@example.org");
    assert(emails[1].second == "example.org");
}

