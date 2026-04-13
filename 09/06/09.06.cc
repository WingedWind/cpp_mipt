#include <cassert>
#include <iterator>

#include <boost/iterator/iterator_facade.hpp>

class Iterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = const int*;
    using reference = const int&;

    Iterator() = default;

    Iterator operator++(int)
    {
        auto copy = *this;
        ++(*this);
        return copy;
    }

    Iterator& operator++()
    {
        const auto next = m_previous + m_current;
        m_previous = m_current;
        m_current = next;
        return *this;
    }

    int operator*() const
    {
        return m_previous;
    }

    friend bool operator==(const Iterator& lhs, const Iterator& rhs)
    {
        return lhs.m_previous == rhs.m_previous && lhs.m_current == rhs.m_current;
    }

private:
    int m_previous = 0;
    int m_current = 1;
};

class FacadeIterator : public boost::iterator_facade<FacadeIterator, int, boost::forward_traversal_tag, int>
{
public:
    FacadeIterator() = default;

private:
    friend class boost::iterator_core_access;

    void increment()
    {
        const auto next = m_previous + m_current;
        m_previous = m_current;
        m_current = next;
    }

    int dereference() const
    {
        return m_previous;
    }

    bool equal(const FacadeIterator& other) const
    {
        return m_previous == other.m_previous && m_current == other.m_current;
    }

    int m_previous = 0;
    int m_current = 1;
};

int fibonacci_v1(int index)
{
    auto iterator = Iterator();

    for (auto i = 0; i < index; ++i)
    {
        ++iterator;
    }

    return *iterator;
}

int fibonacci_v2(int index)
{
    auto iterator = FacadeIterator();

    for (auto i = 0; i < index; ++i)
    {
        ++iterator;
    }

    return *iterator;
}

int main()
{
    Iterator iterator;
    assert(*iterator == 0);
    assert(*iterator++ == 0);
    assert(*iterator == 1);
    assert(*++iterator == 1);
    assert(fibonacci_v1(10) == 55);

    FacadeIterator facade;
    assert(*facade == 0);
    ++facade;
    assert(*facade == 1);
    assert(fibonacci_v2(10) == 55);
}
