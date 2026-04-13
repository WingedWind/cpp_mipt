#include <cassert>
#include <iterator>
#include <memory>
#include <vector>

template<typename T>
class List
{
private:
    struct Node
    {
        explicit Node(T new_value)
            : value(std::move(new_value))
        {
        }

        T value = T();
        std::weak_ptr<Node> prev;
        std::shared_ptr<Node> next;
    };

public:
    class Iterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(std::shared_ptr<Node> node = nullptr, std::shared_ptr<Node> tail = nullptr)
            : m_node(std::move(node))
            , m_tail(std::move(tail))
        {
        }

        Iterator operator++(int)
        {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        Iterator& operator++()
        {
            m_node = m_node->next;
            return *this;
        }

        Iterator operator--(int)
        {
            auto copy = *this;
            --(*this);
            return copy;
        }

        Iterator& operator--()
        {
            if (m_node)
            {
                m_node = m_node->prev.lock();
            }
            else
            {
                m_node = m_tail;
            }

            return *this;
        }

        reference operator*() const
        {
            return m_node->value;
        }

        pointer operator->() const
        {
            return &m_node->value;
        }

        friend bool operator==(const Iterator& lhs, const Iterator& rhs)
        {
            return lhs.m_node == rhs.m_node;
        }

    private:
        std::shared_ptr<Node> m_node;
        std::shared_ptr<Node> m_tail;
    };

    Iterator begin() const
    {
        return Iterator(m_head, m_tail);
    }

    Iterator end() const
    {
        return Iterator(nullptr, m_tail);
    }

    void push_back(T value)
    {
        auto node = std::make_shared<Node>(std::move(value));

        if (!m_head)
        {
            m_head = node;
            m_tail = node;
            return;
        }

        node->prev = m_tail;
        m_tail->next = node;
        m_tail = std::move(node);
    }

private:
    std::shared_ptr<Node> m_head;
    std::shared_ptr<Node> m_tail;
};

int main()
{
    List<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    std::vector<int> forward;
    for (auto value : list)
    {
        forward.push_back(value);
    }

    assert((forward == std::vector<int>{1, 2, 3}));

    auto iterator = list.end();
    assert(*(--iterator) == 3);
    assert(*(--iterator) == 2);
    assert(*(iterator--) == 2);
    assert(*iterator == 1);
}
