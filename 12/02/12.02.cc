#include <cstdio>

void print_escaped(const char* line)
{
    const char quote = '"';
    std::printf("        %c", quote);

    for (const char* current = line; *current != '\0'; ++current)
    {
        if (*current == '\\' || *current == quote)
        {
            std::printf("\\");
        }

        std::printf("%c", *current);
    }

    std::printf("%c,\n", quote);
}

int main()
{
    const char* lines[] = {
        "#include <cstdio>",
        "",
        "void print_escaped(const char* line)",
        "{",
        "    const char quote = '\"';",
        "    std::printf(\"        %c\", quote);",
        "",
        "    for (const char* current = line; *current != '\\0'; ++current)",
        "    {",
        "        if (*current == '\\\\' || *current == quote)",
        "        {",
        "            std::printf(\"\\\\\");",
        "        }",
        "",
        "        std::printf(\"%c\", *current);",
        "    }",
        "",
        "    std::printf(\"%c,\\n\", quote);",
        "}",
        "",
        "int main()",
        "{",
        "    const char* lines[] = {",
        "    };",
        "",
        "    constexpr int declaration_line_count = 23;",
        "    const int line_count = sizeof(lines) / sizeof(lines[0]);",
        "",
        "    for (int i = 0; i < declaration_line_count; ++i)",
        "    {",
        "        std::printf(\"%s\\n\", lines[i]);",
        "    }",
        "",
        "    for (const char* line : lines)",
        "    {",
        "        print_escaped(line);",
        "    }",
        "",
        "    for (int i = declaration_line_count; i < line_count; ++i)",
        "    {",
        "        std::printf(\"%s\\n\", lines[i]);",
        "    }",
        "}",
    };

    constexpr int declaration_line_count = 23;
    const int line_count = sizeof(lines) / sizeof(lines[0]);

    for (int i = 0; i < declaration_line_count; ++i)
    {
        std::printf("%s\n", lines[i]);
    }

    for (const char* line : lines)
    {
        print_escaped(line);
    }

    for (int i = declaration_line_count; i < line_count; ++i)
    {
        std::printf("%s\n", lines[i]);
    }
}
