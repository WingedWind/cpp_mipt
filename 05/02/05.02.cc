#include <iostream>
#include <string>

struct Entity {
    virtual std::string name() const = 0;
    virtual ~Entity() = default;
};

struct Player : virtual Entity {
    std::string name() const override {
        return "Player";
    }
};

template <typename T>
struct Decorator : virtual Entity, T {
};

struct WithHealth {
    int health() const { return 100; }
};

struct WithMana {
    int mana() const { return 50; }
};

struct PlayerWithHealth : Decorator<Player>, WithHealth {};
struct PlayerWithHealthAndMana : Decorator<Player>, WithHealth, WithMana {};

int main() {
    PlayerWithHealth p1;
    PlayerWithHealthAndMana p2;

    std::cout << p1.name() << ", HP=" << p1.health() << '\n';
    std::cout << p2.name()
              << ", HP=" << p2.health()
              << ", Mana=" << p2.mana() << '\n';
}

