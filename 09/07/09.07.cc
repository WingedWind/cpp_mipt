#include "entity.hpp"

#include <cassert>

int main()
{
    Entity entity;
    entity.test();

    Entity moved = std::move(entity);
    moved.test();

    Entity assigned;
    assigned = std::move(moved);
    assigned.test();

    assert(assigned.get() != nullptr);
}
