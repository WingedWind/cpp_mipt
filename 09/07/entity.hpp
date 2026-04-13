#pragma once

#include <array>
#include <bit>
#include <cstddef>

class Entity
{
public:
    Entity();
    Entity(Entity&& other);
    ~Entity();

    Entity& operator=(Entity&& other);

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    void test() const;

private:
    class Implementation;

public:
    auto get() -> Implementation*;
    auto get() const -> const Implementation*;

private:
    alignas(std::max_align_t) std::array<std::byte, 16> m_storage {};
};
