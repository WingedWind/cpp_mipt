#include <cassert>
#include <cstddef>
#include <iostream>
#include <memory>
#include <new>
#include <vector>

class Allocator
{
public:
    virtual ~Allocator() = default;
    virtual void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) = 0;
    virtual void deallocate(void* pointer) = 0;

protected:
    template<typename T>
    T* get(void* pointer) const
    {
        return reinterpret_cast<T*>(pointer);
    }
};

class LinearAllocator : public Allocator
{
public:
    explicit LinearAllocator(std::size_t size)
        : m_size(size)
    {
        m_begin = ::operator new(m_size, std::align_val_t(alignof(std::max_align_t)));
    }

    ~LinearAllocator() override
    {
        ::operator delete(m_begin, m_size, std::align_val_t(alignof(std::max_align_t)));
    }

    void* allocate(std::size_t size, std::size_t alignment) override
    {
        void* begin = get<std::byte>(m_begin) + m_offset;
        auto free = m_size - m_offset;

        if (begin = std::align(alignment, size, begin, free); begin)
        {
            m_offset = m_size - free + size;
            return begin;
        }

        return nullptr;
    }

    void deallocate(void*) override
    {
    }

private:
    std::size_t m_size = 0;
    std::size_t m_offset = 0;
    void* m_begin = nullptr;
};

class StackAllocator : public Allocator
{
public:
    explicit StackAllocator(std::size_t size)
        : m_size(size)
    {
        m_begin = ::operator new(m_size, std::align_val_t(alignof(std::max_align_t)));
    }

    ~StackAllocator() override
    {
        ::operator delete(m_begin, m_size, std::align_val_t(alignof(std::max_align_t)));
    }

    void* allocate(std::size_t size, std::size_t alignment) override
    {
        void* begin = get<std::byte>(m_begin) + m_offset + sizeof(Header);
        auto free = m_size - m_offset - sizeof(Header);

        if (begin = std::align(alignment, size, begin, free); begin)
        {
            auto* header = get<Header>(get<std::byte>(begin) - sizeof(Header));
            header->padding = static_cast<unsigned char>(get<std::byte>(begin) - (get<std::byte>(m_begin) + m_offset));
            m_offset = static_cast<std::size_t>(get<std::byte>(begin) - get<std::byte>(m_begin) + size);
            return begin;
        }

        return nullptr;
    }

    void deallocate(void* pointer) override
    {
        auto* header = get<Header>(get<std::byte>(pointer) - sizeof(Header));
        m_offset = static_cast<std::size_t>(get<std::byte>(pointer) - get<std::byte>(m_begin) - header->padding);
    }

private:
    struct Header
    {
        unsigned char padding = 0;
    };

    std::size_t m_size = 0;
    std::size_t m_offset = 0;
    void* m_begin = nullptr;
};

class PoolAllocator : public Allocator
{
public:
    PoolAllocator(std::size_t size, std::size_t step)
        : m_size(size)
        , m_step(step)
    {
        assert(m_size % m_step == 0 && m_step >= sizeof(Node));
        make_list();
    }

    ~PoolAllocator() override
    {
        for (auto* list : m_lists)
        {
            ::operator delete(list, m_size, std::align_val_t(alignof(std::max_align_t)));
        }
    }

    void* allocate(std::size_t size, std::size_t) override
    {
        assert(size <= m_step);

        if (!m_head)
        {
            if (m_offset == m_lists.size())
            {
                make_list();
            }
            else
            {
                m_head = get<Node>(m_lists[m_offset++]);
            }
        }

        auto* node = m_head;

        if (node->next)
        {
            m_head = node->next;
        }
        else
        {
            auto* next = get<std::byte>(node) + m_step;
            auto* list_end = get<std::byte>(m_lists[m_offset - 1]) + m_size;
            m_head = (next == list_end) ? nullptr : get<Node>(next);
            if (m_head)
            {
                m_head->next = nullptr;
            }
        }

        return node;
    }

    void deallocate(void* pointer) override
    {
        auto* node = get<Node>(pointer);
        node->next = m_head;
        m_head = node;
    }

private:
    struct Node
    {
        Node* next = nullptr;
    };

    void make_list()
    {
        auto* head = ::operator new(m_size, std::align_val_t(alignof(std::max_align_t)));
        m_lists.push_back(head);
        m_head = get<Node>(head);
        m_head->next = nullptr;
        ++m_offset;
    }

    std::size_t m_size = 0;
    std::size_t m_step = 0;
    std::size_t m_offset = 0;
    Node* m_head = nullptr;
    std::vector<void*> m_lists;
};

class FreeListAllocator : public Allocator
{
public:
    explicit FreeListAllocator(std::size_t size)
        : m_size(size)
    {
        m_begin = ::operator new(m_size, std::align_val_t(alignof(std::max_align_t)));
        m_head = get<Node>(m_begin);
        m_head->size = m_size - sizeof(Header);
        m_head->next = nullptr;
    }

    ~FreeListAllocator() override
    {
        ::operator delete(m_begin, m_size, std::align_val_t(alignof(std::max_align_t)));
    }

    void* allocate(std::size_t size, std::size_t alignment) override
    {
        void* end = get<std::byte>(m_begin) + sizeof(Header) + size;
        void* next = end;
        auto free = 2 * alignment;

        if (next = std::align(alignment, sizeof(Header), next, free); !next)
        {
            return nullptr;
        }

        const auto padding = static_cast<std::size_t>(get<std::byte>(next) - get<std::byte>(end));

        Node* current = m_head;
        Node* previous = nullptr;

        while (current && current->size < size + padding)
        {
            previous = current;
            current = current->next;
        }

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

    void deallocate(void* pointer) override
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

    std::size_t m_size = 0;
    void* m_begin = nullptr;
    Node* m_head = nullptr;
};

int main()
{
    std::vector<std::unique_ptr<Allocator>> allocators;
    allocators.push_back(std::make_unique<LinearAllocator>(256));
    allocators.push_back(std::make_unique<StackAllocator>(256));
    allocators.push_back(std::make_unique<PoolAllocator>(128, 16));
    allocators.push_back(std::make_unique<FreeListAllocator>(256));

    for (auto& allocator : allocators)
    {
        auto* pointer = allocator->allocate(16, alignof(std::max_align_t));
        assert(pointer != nullptr);
        allocator->deallocate(pointer);
    }

    std::cout << "All polymorphic allocators allocated and deallocated memory\n";
}
