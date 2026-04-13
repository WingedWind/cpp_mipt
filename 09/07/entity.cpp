#include "entity.hpp"

#include <iostream>
#include <memory>
#include <new>
#include <utility>

class Entity::Implementation
{
public:
    explicit Implementation(int value = 0)
        : m_value(value)
    {
    }

    void test() const
    {
        std::cout << "Implementation::test : value = " << m_value << '\n';
    }

    Implementation(const Implementation&) = default;
    Implementation(Implementation&&) = default;
    Implementation& operator=(const Implementation&) = default;
    Implementation& operator=(Implementation&&) = default;

    int m_value = 0;
};

Entity::Entity()
{
    static_assert(sizeof(Implementation) <= 16);
    static_assert(alignof(Implementation) <= alignof(std::max_align_t));

    new (m_storage.data()) Implementation(42);
}

Entity::Entity(Entity&& other)
{
    static_assert(sizeof(Implementation) <= 16);
    static_assert(alignof(Implementation) <= alignof(std::max_align_t));

    new (m_storage.data()) Implementation(*other.get());
}

Entity::~Entity()
{
    std::destroy_at(get());
}

Entity& Entity::operator=(Entity&& other)
{
    if (this != &other)
    {
        *get() = std::move(*other.get());
    }

    return *this;
}

void Entity::test() const
{
    get()->test();
}

auto Entity::get() -> Implementation*
{
    return std::launder(std::bit_cast<Implementation*>(m_storage.data()));
}

auto Entity::get() const -> const Implementation*
{
    return std::launder(std::bit_cast<const Implementation*>(m_storage.data()));
}
