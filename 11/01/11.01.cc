#include <cassert>
#include <type_traits>

class Wrapper;

Wrapper test();

class Wrapper
{
public:
    using pointer = Wrapper (*)();

    explicit Wrapper(pointer function = nullptr)
        : m_function(function)
    {
    }

    operator pointer() const
    {
        return m_function;
    }

private:
    pointer m_function = nullptr;
};

Wrapper test()
{
    return Wrapper(&test);
}

int main()
{
    static_assert(std::is_same_v<decltype(test), Wrapper()>);
    static_assert(std::is_same_v<decltype(&test), Wrapper (*)()>);

    Wrapper function = test();
    Wrapper next = (*function)();

    assert(static_cast<Wrapper::pointer>(function) == &test);
    assert(static_cast<Wrapper::pointer>(next) == &test);
}
