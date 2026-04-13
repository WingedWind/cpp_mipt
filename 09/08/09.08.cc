#include <cassert>
#include <cstddef>
#include <iostream>
#include <new>

template<typename Derived>
class Entity
{
public:
    static void* operator new(std::size_t size)
    {
        std::cout << "Entity::operator new\n";
        return ::operator new(size);
    }

    static void operator delete(void* pointer)
    {
        std::cout << "Entity::operator delete\n";
        ::operator delete(pointer);
    }

    static void* operator new[](std::size_t size)
    {
        std::cout << "Entity::operator new[]\n";
        return ::operator new[](size);
    }

    static void operator delete[](void* pointer)
    {
        std::cout << "Entity::operator delete[]\n";
        ::operator delete[](pointer);
    }

    static void* operator new(std::size_t size, const std::nothrow_t& tag) noexcept
    {
        std::cout << "Entity::operator new nothrow\n";
        return ::operator new(size, tag);
    }

    static void operator delete(void* pointer, const std::nothrow_t&) noexcept
    {
        std::cout << "Entity::operator delete nothrow\n";
        ::operator delete(pointer);
    }

    static void* operator new[](std::size_t size, const std::nothrow_t& tag) noexcept
    {
        std::cout << "Entity::operator new[] nothrow\n";
        return ::operator new[](size, tag);
    }

    static void operator delete[](void* pointer, const std::nothrow_t&) noexcept
    {
        std::cout << "Entity::operator delete[] nothrow\n";
        ::operator delete[](pointer);
    }

protected:
    Entity() = default;
};

class Client : private Entity<Client>
{
public:
    Client()
    {
        ++instances;
    }

    ~Client()
    {
        --instances;
    }

    using Entity<Client>::operator new;
    using Entity<Client>::operator delete;
    using Entity<Client>::operator new[];
    using Entity<Client>::operator delete[];

    static inline int instances = 0;
};

int main()
{
    auto* client = new Client;
    delete client;

    auto* array = new Client[3];
    delete[] array;

    auto* nothrow_client = new (std::nothrow) Client;
    assert(nothrow_client != nullptr);
    delete nothrow_client;

    auto* nothrow_array = new (std::nothrow) Client[2];
    assert(nothrow_array != nullptr);
    delete[] nothrow_array;

    assert(Client::instances == 0);
}
