#pragma once

#include <cstddef>
#include <new>
#include <utility>
#include <algorithm> // Для std::max
#include <iostream>

template<class T, std::size_t BlockSize = sizeof(T)>
class ExpandablePoolAllocator {
public:
    using value_type = T;
    using size_type  = std::size_t;
    using pointer    = T*;

    template<class U>
    struct [[maybe_unused]] rebind {
        using other = ExpandablePoolAllocator<U, BlockSize>;
    };

    ExpandablePoolAllocator() = default;

    template<class U>
    [[maybe_unused]] constexpr explicit ExpandablePoolAllocator(const ExpandablePoolAllocator<U, BlockSize>&) noexcept {}

    ~ExpandablePoolAllocator() noexcept {
        release_all();
    }

    [[nodiscard]] T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n) noexcept;

    [[maybe_unused]] [[nodiscard]] std::size_t used_blocks() const noexcept { return used_; }
    [[maybe_unused]] [[nodiscard]] std::size_t total_blocks() const noexcept { return total_; }

private:
    struct Node {
        Node* next;
    };

    struct Chunk {
        Chunk* next;
    };

    Node* free_list_  = nullptr;
    Chunk* chunks_     = nullptr;
    size_t used_       = 0;
    size_t total_      = 0;

    void grow(std::size_t additional_blocks);
    void release_all() noexcept;
};


template<class T, std::size_t BlockSize>
void ExpandablePoolAllocator<T, BlockSize>::grow(std::size_t additional_blocks) {
    if (additional_blocks == 0) {
        return;
    }

    constexpr size_t ActualBlockSize = std::max(BlockSize, sizeof(Node));

    size_t bytes_to_alloc = sizeof(Chunk) + additional_blocks * ActualBlockSize;
    void* mem = ::operator new(bytes_to_alloc);

    auto* new_chunk = static_cast<Chunk*>(mem);
    new_chunk->next = chunks_;
    chunks_ = new_chunk;

    void* blocks_mem = static_cast<void*>(new_chunk + 1);

    for (std::size_t i = 0; i < additional_blocks; ++i) {
        auto* node = reinterpret_cast<Node*>(
                static_cast<std::byte*>(blocks_mem) + i * ActualBlockSize
        );
        node->next = free_list_;
        free_list_ = node;
    }
    total_ += additional_blocks;
}

template<class T, std::size_t BlockSize>
T* ExpandablePoolAllocator<T, BlockSize>::allocate(std::size_t n) {
    if (n == 0) {
        return nullptr;
    }

    constexpr size_t ActualBlockSize = std::max(BlockSize, sizeof(Node));

    const std::size_t needed_bytes = n * sizeof(T);
    const std::size_t needed_blocks = (needed_bytes + ActualBlockSize - 1) / ActualBlockSize;

    std::size_t have_blocks = 0;
    for (Node* cur = free_list_; cur && (have_blocks < needed_blocks); cur = cur->next) {
        ++have_blocks;
    }

    if (have_blocks < needed_blocks) {
        std::size_t grow_by = std::max<std::size_t>(64, needed_blocks - have_blocks);
        grow(grow_by);
    }

    if (needed_blocks > 1) {
        return static_cast<T*>(::operator new(needed_bytes));
    }

    Node* head = free_list_;
    free_list_ = head->next;

    used_ += needed_blocks;
    return reinterpret_cast<T*>(head);
}


template<class T, std::size_t BlockSize>
void ExpandablePoolAllocator<T, BlockSize>::deallocate(T* p, std::size_t n) noexcept {
    if (!p || n == 0) {
        return;
    }

    constexpr size_t ActualBlockSize = std::max(BlockSize, sizeof(Node));

    const std::size_t needed_bytes = n * sizeof(T);
    const std::size_t needed_blocks = (needed_bytes + ActualBlockSize - 1) / ActualBlockSize;

    if (needed_blocks > 1) {
        ::operator delete(p);
        return;
    }

    auto* node = reinterpret_cast<Node*>(p);
    node->next = free_list_;
    free_list_ = node;

    used_ -= needed_blocks;
}

template<class T, std::size_t BlockSize>
void ExpandablePoolAllocator<T, BlockSize>::release_all() noexcept {
    Chunk* current = chunks_;
    while (current) {
        Chunk* next = current->next;
        ::operator delete(current);
        current = next;
    }
    chunks_ = nullptr;
    free_list_ = nullptr;
    used_ = total_ = 0;
}

template <class T, class U, size_t BlockSize>
constexpr bool operator==(const ExpandablePoolAllocator<T, BlockSize>&, const ExpandablePoolAllocator<U, BlockSize>&) noexcept {
    return true;
}

template <class T, class U, size_t BlockSize>
constexpr bool operator!=(const ExpandablePoolAllocator<T, BlockSize>&, const ExpandablePoolAllocator<U, BlockSize>&) noexcept {
    return false;
}
