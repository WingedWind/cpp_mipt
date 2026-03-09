#include <iostream>

class Entity_v1 {
private:
    int value;

public:
    Entity_v1(int v = 0) : value(v) {}

    void print() const {
        std::cout << "Entity_v1.value = " << value << std::endl;
    }
};

class Entity_v2 {
public:
    int value;
};

void hack_with_reinterpret(Entity_v1& obj, int newValue) {
    Entity_v2& fake = reinterpret_cast<Entity_v2&>(obj);
    fake.value = newValue;
}

void hack_with_raw_memory(Entity_v1& obj, int newValue) {
    int* ptr = reinterpret_cast<int*>(&obj);
    *ptr = newValue;
}

int main() {
    Entity_v1 e(10);

    std::cout << "Initial state:\n";
    e.print();

    std::cout << "\nAfter hack_with_reinterpret:\n";
    hack_with_reinterpret(e, 123);
    e.print();

    std::cout << "\nAfter hack_with_raw_memory:\n";
    hack_with_raw_memory(e, 999);
    e.print();

    return 0;
}