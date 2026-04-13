#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <new>
#include <random>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>

class Allocator
{
public:
    enum class Policy
    {
        first_fit,
        best_fit
    };

    explicit Allocator(std::size_t size, Policy policy = Policy::first_fit)
        : m_size(size)
        , m_policy(policy)
    {
        assert(m_size >= sizeof(Node) + sizeof(Header) + 1);

        m_begin = ::operator new(m_size, std::align_val_t(alignof(std::max_align_t)));
        m_head = get<Node>(m_begin);
        m_head->size = m_size - sizeof(Header);
        m_head->next = nullptr;
    }

    ~Allocator()
    {
        ::operator delete(m_begin, m_size, std::align_val_t(alignof(std::max_align_t)));
    }

    void* allocate(std::size_t size)
    {
        void* end = get<std::byte>(m_begin) + sizeof(Header) + size;
        void* next = end;
        auto free = 2 * alignof(Header);

        if (next = std::align(alignof(Header), sizeof(Header), next, free); !next)
        {
            return nullptr;
        }

        const auto padding = static_cast<std::size_t>(get<std::byte>(next) - get<std::byte>(end));
        auto [current, previous] = (m_policy == Policy::first_fit) ? find_first(size + padding) : find_best(size + padding);

        if (!current)
        {
            return nullptr;
        }

        auto total = size + padding;
        if (current->size >= total + sizeof(Node) + 1)
        {
            const auto step = sizeof(Header) + total;
            auto* node = get<Node>(get<std::byte>(current) + step);
            node->size = current->size - step;
            node->next = current->next;
            current->next = node;
        }
        else
        {
            total = current->size;
        }

        if (!previous)
        {
            m_head = current->next;
        }
        else
        {
            previous->next = current->next;
        }

        auto* header = get<Header>(current);
        header->size = total;

        return get<std::byte>(current) + sizeof(Header);
    }

    void deallocate(void* pointer)
    {
        auto* node = get<Node>(get<std::byte>(pointer) - sizeof(Header));

        Node* previous = nullptr;
        Node* current = m_head;

        while (current && current < node)
        {
            previous = current;
            current = current->next;
        }

        node->next = current;

        if (!previous)
        {
            m_head = node;
        }
        else
        {
            previous->next = node;
        }

        merge(previous, node);
    }

private:
    struct Node
    {
        std::size_t size = 0;
        Node* next = nullptr;
    };

    struct alignas(std::max_align_t) Header
    {
        std::size_t size = 0;
    };

    template<typename T>
    T* get(void* pointer) const
    {
        return reinterpret_cast<T*>(pointer);
    }

    auto find_first(std::size_t size) const -> std::pair<Node*, Node*>
    {
        Node* current = m_head;
        Node* previous = nullptr;

        while (current && current->size < size)
        {
            previous = current;
            current = current->next;
        }

        return {current, previous};
    }

    auto find_best(std::size_t size) const -> std::pair<Node*, Node*>
    {
        Node* current = m_head;
        Node* previous = nullptr;
        Node* best = nullptr;
        Node* best_previous = nullptr;

        while (current)
        {
            if (current->size >= size && (!best || current->size < best->size))
            {
                best = current;
                best_previous = previous;
            }

            previous = current;
            current = current->next;
        }

        return {best, best_previous};
    }

    void merge(Node* previous, Node* node)
    {
        if (node->next && get<std::byte>(node) + sizeof(Header) + node->size == get<std::byte>(node->next))
        {
            node->size += sizeof(Header) + node->next->size;
            node->next = node->next->next;
        }

        if (previous && get<std::byte>(previous) + sizeof(Header) + previous->size == get<std::byte>(node))
        {
            previous->size += sizeof(Header) + node->size;
            previous->next = node->next;
        }
    }

    std::size_t m_size = 0;
    Policy m_policy = Policy::first_fit;
    void* m_begin = nullptr;
    Node* m_head = nullptr;
};

static std::vector<std::size_t> make_sizes(std::size_t count)
{
    std::vector<std::size_t> sizes(count);
    std::minstd_rand engine(42);
    std::uniform_int_distribution<std::size_t> distribution(8, 128);

    for (auto& size : sizes)
    {
        size = distribution(engine);
    }

    return sizes;
}

static void benchmark_policy(benchmark::State& state, Allocator::Policy policy)
{
    const auto sizes = make_sizes(1024);
    std::vector<void*> pointers(sizes.size(), nullptr);

    for (auto _ : state)
    {
        Allocator allocator(1 << 20, policy);

        for (auto i = 0uz; i < sizes.size(); ++i)
        {
            pointers[i] = allocator.allocate(sizes[i]);
        }

        for (auto i = 0uz; i < sizes.size(); i += 3)
        {
            allocator.deallocate(pointers[i]);
        }

        for (auto i = 0uz; i < sizes.size(); i += 3)
        {
            pointers[i] = allocator.allocate(sizes[i] / 2 + 1);
        }

        for (auto pointer : pointers)
        {
            allocator.deallocate(pointer);
        }

        benchmark::DoNotOptimize(pointers);
    }
}

static void first_fit_benchmark(benchmark::State& state)
{
    benchmark_policy(state, Allocator::Policy::first_fit);
}

static void best_fit_benchmark(benchmark::State& state)
{
    benchmark_policy(state, Allocator::Policy::best_fit);
}

BENCHMARK(first_fit_benchmark);
BENCHMARK(best_fit_benchmark);

int main(int argc, char** argv)
{
    Allocator first(256, Allocator::Policy::first_fit);
    Allocator best(256, Allocator::Policy::best_fit);

    auto* a = first.allocate(32);
    auto* b = first.allocate(16);
    first.deallocate(a);
    first.deallocate(b);

    auto* c = best.allocate(32);
    auto* d = best.allocate(16);
    best.deallocate(c);
    best.deallocate(d);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}
