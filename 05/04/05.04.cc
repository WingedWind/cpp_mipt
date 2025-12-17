#include <iostream>

class WalkStrategy {
public:
    void execute() const {
        std::cout << "Walking" << std::endl;
    }
};

class RunStrategy {
public:
    void execute() const {
        std::cout << "Running" << std::endl;
    }
};

class FlyStrategy {
public:
    void execute() const {
        std::cout << "Flying" << std::endl;
    }
};

template <typename TStrategy>
class Entity : public TStrategy {
public:
    using TStrategy::execute;
    void performAction() const {
        execute();
    }

    void demonstrate() const {
        std::cout << "Entity is ";
        execute();
    }
};

int main() {
    Entity<WalkStrategy> walker;
    Entity<RunStrategy> runner;
    Entity<FlyStrategy> flyer;

    std::cout << "Using static polymorphism Strategy pattern:\n";

    walker.demonstrate();  // Entity is Walking
    runner.demonstrate();  // Entity is Running
    flyer.demonstrate();   // Entity is Flying

    std::cout << "\nDirect strategy calls:\n";
    walker.execute();  // Walking
    runner.execute();  // Running
    flyer.execute();   // Flying

    std::cout << "\nThrough Entity interface:\n";
    walker.performAction();  // Walking
    runner.performAction();  // Running
    flyer.performAction();   // Flying

    return 0;
}
