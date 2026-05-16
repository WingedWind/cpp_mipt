#include <cassert>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

bool is_blank_line(const std::string& line)
{
    return line.find_first_not_of(" \t\r\n") == std::string::npos;
}

std::string remove_comments(std::string_view source)
{
    std::string result;

    for (auto position = 0uz; position < source.size();)
    {
        if (position + 1 < source.size() && source[position] == 'R' && source[position + 1] == '"')
        {
            const auto delimiter_begin = position + 2;
            const auto open = source.find('(', delimiter_begin);

            if (open != std::string_view::npos)
            {
                const auto delimiter = source.substr(delimiter_begin, open - delimiter_begin);
                const auto close = std::string(")") + std::string(delimiter) + "\"";
                const auto raw_end = source.find(close, open + 1);
                const auto end = raw_end == std::string_view::npos ? source.size() : raw_end + close.size();

                result.append(source.substr(position, end - position));
                position = end;
                continue;
            }
        }

        if (source[position] == '\'')
        {
            result.push_back(source[position++]);

            while (position < source.size())
            {
                result.push_back(source[position]);

                if (source[position] == '\'' && source[position - 1] != '\\')
                {
                    ++position;
                    break;
                }

                ++position;
            }

            continue;
        }

        if (source[position] == '"')
        {
            result.push_back(source[position++]);

            while (position < source.size())
            {
                result.push_back(source[position]);

                if (source[position] == '"' && source[position - 1] != '\\')
                {
                    ++position;
                    break;
                }

                ++position;
            }

            continue;
        }

        if (position + 1 < source.size() && source[position] == '/' && source[position + 1] == '/')
        {
            position += 2;

            while (position < source.size() && source[position] != '\n')
            {
                ++position;
            }

            continue;
        }

        if (position + 1 < source.size() && source[position] == '/' && source[position + 1] == '*')
        {
            position += 2;

            while (position + 1 < source.size() && !(source[position] == '*' && source[position + 1] == '/'))
            {
                ++position;
            }

            position = std::min(position + 2, source.size());
            continue;
        }

        result.push_back(source[position++]);
    }

    return result;
}

std::string remove_blank_lines(std::string source)
{
    std::stringstream input(source);
    std::stringstream output;
    std::string line;
    auto inside_raw_string = false;
    std::string raw_close;

    while (std::getline(input, line))
    {
        if (inside_raw_string)
        {
            output << line << '\n';

            if (line.find(raw_close) != std::string::npos)
            {
                inside_raw_string = false;
            }

            continue;
        }

        const auto raw_begin = line.find("R\"");
        const auto raw_open = raw_begin == std::string::npos ? std::string::npos : line.find('(', raw_begin + 2);

        if (raw_open != std::string::npos)
        {
            raw_close = ")" + line.substr(raw_begin + 2, raw_open - raw_begin - 2) + "\"";
            inside_raw_string = line.find(raw_close, raw_open + 1) == std::string::npos;
        }

        if (!is_blank_line(line) || inside_raw_string)
        {
            output << line << '\n';
        }
    }

    return output.str();
}

void transform(const std::filesystem::path& input_path, const std::filesystem::path& output_path)
{
    std::fstream input(input_path, std::ios::in);
    const auto source = (std::stringstream() << input.rdbuf()).str();
    std::fstream(output_path, std::ios::out) << remove_blank_lines(remove_comments(source));
}

int main()
{
    const auto input_path = std::filesystem::temp_directory_path() / "blank_lines_input.cpp";
    const auto output_path = std::filesystem::temp_directory_path() / "blank_lines_output.cpp";

    std::fstream(input_path, std::ios::out) << "int main()\n"
                                               "{\n"
                                               "    // remove this comment\n"
                                               "\n"
                                               "    auto text = R\"(\n"
                                               "\n"
                                               "// keep this raw string content\n"
                                               "value\n"
                                               ")\";\n"
                                               "    auto other = 1; /* remove this comment */\n"
                                               "   \n"
                                               "}\n";

    transform(input_path, output_path);

    std::fstream output(output_path, std::ios::in);
    const auto result = (std::stringstream() << output.rdbuf()).str();

    assert(result.find("remove this comment") == std::string::npos);
    assert(result.find("/*") == std::string::npos);
    assert(result.find("\n\n// keep this raw string content") != std::string::npos);
    assert(result.find("value") != std::string::npos);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}
