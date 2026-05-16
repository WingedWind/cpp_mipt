#include <cassert>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

std::vector<std::filesystem::path> show(const std::filesystem::path& path, const std::regex& pattern)
{
    std::vector<std::filesystem::path> matches;

    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
    {
        return matches;
    }

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        const auto filename = entry.path().filename().string();

        if (std::regex_search(filename, pattern))
        {
            matches.push_back(entry.path());
        }
    }

    return matches;
}

int main()
{
    const auto directory = std::filesystem::temp_directory_path() / "directory_grep_test";
    std::filesystem::create_directories(directory);
    std::fstream(directory / "main.cpp", std::ios::out);
    std::fstream(directory / "notes.txt", std::ios::out);
    std::fstream(directory / "test.cpp", std::ios::out);

    const auto matches = show(directory, std::regex(R"(\.cpp$)"));

    assert(matches.size() == 2);
    assert(std::any_of(matches.begin(), matches.end(), [](const auto& path) { return path.filename() == "main.cpp"; }));
    assert(std::any_of(matches.begin(), matches.end(), [](const auto& path) { return path.filename() == "test.cpp"; }));

    std::filesystem::remove_all(directory);
}
