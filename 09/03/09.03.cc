#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace builder_example
{
    struct Entity
    {
        int x = 0;
        int y = 0;
    };

    class Builder
    {
    public:
        virtual ~Builder() = default;

        std::unique_ptr<Entity> make_entity()
        {
            m_entity = std::make_unique<Entity>();
            set_x();
            set_y();
            return std::move(m_entity);
        }

        virtual void set_x() const = 0;
        virtual void set_y() const = 0;

    protected:
        mutable std::unique_ptr<Entity> m_entity;
    };

    class Builder_Client : public Builder
    {
    public:
        void set_x() const override { m_entity->x = 1; }
        void set_y() const override { m_entity->y = 1; }
    };

    void run()
    {
        Builder_Client builder;
        auto entity = builder.make_entity();

        assert(entity->x == 1);
        assert(entity->y == 1);
    }
}

namespace abstract_factory_example
{
    class Entity
    {
    public:
        virtual ~Entity() = default;
        virtual int test() const = 0;
    };

    class Client : public Entity
    {
    public:
        int test() const override { return 1; }
    };

    class Server : public Entity
    {
    public:
        int test() const override { return 2; }
    };

    class Factory
    {
    public:
        virtual ~Factory() = default;
        virtual std::unique_ptr<Entity> make_entity() const = 0;
    };

    class Factory_Client : public Factory
    {
    public:
        std::unique_ptr<Entity> make_entity() const override
        {
            return std::make_unique<Client>();
        }
    };

    class Factory_Server : public Factory
    {
    public:
        std::unique_ptr<Entity> make_entity() const override
        {
            return std::make_unique<Server>();
        }
    };

    void run()
    {
        Factory_Client client_factory;
        Factory_Server server_factory;

        assert(client_factory.make_entity()->test() == 1);
        assert(server_factory.make_entity()->test() == 2);
    }
}

namespace prototype_example
{
    class Entity
    {
    public:
        virtual ~Entity() = default;
        virtual std::unique_ptr<Entity> copy() const = 0;
        virtual int test() const = 0;
    };

    class Client : public Entity
    {
    public:
        std::unique_ptr<Entity> copy() const override
        {
            return std::make_unique<Client>(*this);
        }

        int test() const override { return 1; }
    };

    class Server : public Entity
    {
    public:
        std::unique_ptr<Entity> copy() const override
        {
            return std::make_unique<Server>(*this);
        }

        int test() const override { return 2; }
    };

    class Prototype
    {
    public:
        Prototype()
        {
            m_entities.push_back(std::make_unique<Client>());
            m_entities.push_back(std::make_unique<Server>());
        }

        std::unique_ptr<Entity> make_client() const
        {
            return m_entities.at(0)->copy();
        }

        std::unique_ptr<Entity> make_server() const
        {
            return m_entities.at(1)->copy();
        }

    private:
        std::vector<std::unique_ptr<Entity>> m_entities;
    };

    void run()
    {
        Prototype prototype;

        assert(prototype.make_client()->test() == 1);
        assert(prototype.make_server()->test() == 2);
    }
}

namespace composite_example
{
    class Entity
    {
    public:
        virtual ~Entity() = default;
        virtual int test() const = 0;
    };

    class Client : public Entity
    {
    public:
        int test() const override { return 1; }
    };

    class Server : public Entity
    {
    public:
        int test() const override { return 2; }
    };

    class Composite : public Entity
    {
    public:
        void add(std::shared_ptr<Entity> entity)
        {
            m_entities.push_back(std::move(entity));
        }

        int test() const override
        {
            auto total = 0;

            for (const auto& entity : m_entities)
            {
                total += entity->test();
            }

            return total;
        }

    private:
        std::vector<std::shared_ptr<Entity>> m_entities;
    };

    std::shared_ptr<Entity> make_composite(std::size_t size_1, std::size_t size_2)
    {
        auto composite = std::make_shared<Composite>();

        for (auto i = 0uz; i < size_1; ++i)
        {
            composite->add(std::make_shared<Client>());
        }

        for (auto i = 0uz; i < size_2; ++i)
        {
            composite->add(std::make_shared<Server>());
        }

        return composite;
    }

    void run()
    {
        auto composite = std::make_shared<Composite>();

        for (auto i = 0uz; i < 5; ++i)
        {
            composite->add(make_composite(1, 1));
        }

        std::shared_ptr<Entity> entity = composite;
        assert(entity->test() == 15);
    }
}

namespace observer_example
{
    class Observer
    {
    public:
        virtual ~Observer() = default;
        virtual void test(int value) = 0;
    };

    class Entity
    {
    public:
        void add(std::shared_ptr<Observer> observer)
        {
            m_observers.push_back(std::move(observer));
        }

        void set(int value)
        {
            m_value = value;
            notify_all();
        }

    private:
        void notify_all()
        {
            for (const auto& observer : m_observers)
            {
                observer->test(m_value);
            }
        }

        int m_value = 0;
        std::vector<std::shared_ptr<Observer>> m_observers;
    };

    class Client : public Observer
    {
    public:
        void test(int value) override
        {
            calls += 1;
            last = value;
            std::cout << "Client::test : x = " << value << '\n';
        }

        int calls = 0;
        int last = 0;
    };

    class Server : public Observer
    {
    public:
        void test(int value) override
        {
            calls += 1;
            last = value;
            std::cout << "Server::test : x = " << value << '\n';
        }

        int calls = 0;
        int last = 0;
    };

    void run()
    {
        Entity entity;
        auto client = std::make_shared<Client>();
        auto server = std::make_shared<Server>();

        entity.add(client);
        entity.add(server);

        entity.set(1);
        entity.set(2);

        assert(client->calls == 2);
        assert(server->calls == 2);
        assert(client->last == 2);
        assert(server->last == 2);
    }
}

int main()
{
    builder_example::run();
    abstract_factory_example::run();
    prototype_example::run();
    composite_example::run();
    observer_example::run();
}
