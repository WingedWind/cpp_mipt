#include <iostream>
#include <memory>
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////
// Template Method
//////////////////////////////////////////////////////////////

class Unit {
public:
    virtual ~Unit() = default;

    void performTurn() {
        move();
        attack();
        endTurn();
    }

protected:
    virtual void move() = 0;
    virtual void attack() = 0;
    virtual void endTurn() {
        std::cout << getName() << " ends turn.\n";
    }

public:
    virtual std::string getName() const = 0;
};

//////////////////////////////////////////////////////////////
// Units
//////////////////////////////////////////////////////////////

class Soldier : public Unit {
protected:
    void move() override { std::cout << "Soldier moves.\n"; }
    void attack() override { std::cout << "Soldier attacks.\n"; }

public:
    std::string getName() const override { return "Soldier"; }
};

class Archer : public Unit {
protected:
    void move() override { std::cout << "Archer moves.\n"; }
    void attack() override { std::cout << "Archer shoots an arrow.\n"; }

public:
    std::string getName() const override { return "Archer"; }
};

//////////////////////////////////////////////////////////////
// Builder
//////////////////////////////////////////////////////////////

class UnitBuilder {
public:
    virtual ~UnitBuilder() = default;
    virtual std::unique_ptr<Unit> buildSoldier() = 0;
    virtual std::unique_ptr<Unit> buildArcher() = 0;
};

class SimpleUnitBuilder : public UnitBuilder {
public:
    std::unique_ptr<Unit> buildSoldier() override {
        return std::make_unique<Soldier>();
    }

    std::unique_ptr<Unit> buildArcher() override {
        return std::make_unique<Archer>();
    }
};

//////////////////////////////////////////////////////////////
// Composite
//////////////////////////////////////////////////////////////

class Army : public Unit {
public:
    void add(std::unique_ptr<Unit> unit) {
        units.emplace_back(std::move(unit));
    }

protected:
    void move() override {
        for (auto& u : units) u->performTurn();
    }

    void attack() override {
        for (auto& u : units) u->performTurn();
    }

public:
    std::string getName() const override { return "Army"; }

private:
    std::vector<std::unique_ptr<Unit>> units;
};


int main() {
    SimpleUnitBuilder builder;

    auto soldier = builder.buildSoldier();
    auto archer  = builder.buildArcher();

    Army army;
    army.add(builder.buildSoldier());
    army.add(builder.buildSoldier());
    army.add(builder.buildArcher());

    std::cout << "--- Soldier Turn ---\n";
    soldier->performTurn();

    std::cout << "--- Archer Turn ---\n";
    archer->performTurn();

    std::cout << "--- Army Turn ---\n";
    army.performTurn();

    return 0;
}

