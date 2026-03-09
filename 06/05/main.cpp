#include <functional>
#include <iostream>
#include <string>

#include <boost/dll.hpp>
#include <boost/filesystem/path.hpp>

int main()
{
    std::cout << "Enter dynamic library name\n";
    std::cout << "(for example: libshared_v1.so, libshared_v2.so, "
                 "shared_v1.dll, shared_v2.dll)\n";
    std::cout << "Type 'exit' to quit\n\n";

    std::string library_name;

    while (true)
    {
        std::cout << "library> ";
        if (!(std::cin >> library_name))
        {
            std::cerr << "Input error.\n";
            return 1;
        }

        if (library_name == "exit")
        {
            break;
        }

        try
        {
            const auto full_path =
                boost::dll::program_location().parent_path() / library_name;

            std::function<void()> test_function =
                boost::dll::import_symbol<void()>(full_path, "test");

            std::cout << "Library loaded successfully: "
                      << full_path.string() << '\n';

            test_function();
        }
        catch (std::exception const& e)
        {
            std::cerr << "Failed to load library or import function 'test': "
                      << e.what() << '\n';
            std::cerr << "Try another file name.\n";
        }
        catch (...)
        {
            std::cerr << "Unknown error occurred.\n";
        }

        std::cout << '\n';
    }

    return 0;
}
