#include <iostream>
#include <source_location>

class Tracer
{
public:
    Tracer(std::source_location location = std::source_location::current())
        : m_location(location)
    {
        std::cout
            << "enter " << m_location.function_name()
            << " [" << m_location.file_name() << ':' << m_location.line() << ':' << m_location.column()
            << "]\n";
    }

    ~Tracer()
    {
        std::cout
            << "leave " << m_location.function_name()
            << " [" << m_location.file_name() << ':' << m_location.line() << ':' << m_location.column()
            << "]\n";
    }

private:
    std::source_location m_location;
};

#define TRACE_JOIN_IMPL(lhs, rhs) lhs##rhs
#define TRACE_JOIN(lhs, rhs) TRACE_JOIN_IMPL(lhs, rhs)

#ifndef NDEBUG
#define trace() ::Tracer TRACE_JOIN(tracer_, __LINE__)(std::source_location::current())
#else
#define trace() static_cast<void>(0)
#endif

int factorial(int value)
{
    trace();

    if (value <= 1)
    {
        return 1;
    }

    return value * factorial(value - 1);
}

void demo()
{
    trace();

    std::cout << "factorial(4) = " << factorial(4) << '\n';
}

int main()
{
    trace();
    demo();
}
