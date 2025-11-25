#pragma once

#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class MyList {
private:
    struct Node {
        T data;
        Node* next;
    };

public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        explicit iterator(Node* node) : current_(node) {}

        reference operator*() const { return current_->data; }
        pointer operator->() { return &current_->data; }

        iterator& operator++() {
            current_ = current_->next;
            return *this;
        }

        iterator operator++(int) {
            iterator old = *this;
            ++(*this);
            return old;
        }

        friend bool operator==(const iterator& a, const iterator& b) { return a.current_ == b.current_; };
        friend bool operator!=(const iterator& a, const iterator& b) { return a.current_ != b.current_; };

    private:
        Node* current_;
    };

    MyList() = default;

    ~MyList() {
        clear();
    }

    void push_back(const T& value);
    void clear();

    [[nodiscard]] size_type size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    iterator begin() { return iterator(head_); }
    iterator end() { return iterator(nullptr); }

private:
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_type size_ = 0;
    NodeAllocator node_alloc_{};
};

template<typename T, typename Allocator>
void MyList<T, Allocator>::push_back(const T& value) {
    Node* new_node = std::allocator_traits<NodeAllocator>::allocate(node_alloc_, 1);

    try {
        std::allocator_traits<NodeAllocator>::construct(node_alloc_, new_node, Node{value, nullptr});
    } catch (...) {
        std::allocator_traits<NodeAllocator>::deallocate(node_alloc_, new_node, 1);
        throw;
    }

    if (empty()) {
        head_ = tail_ = new_node;
    } else {
        tail_->next = new_node;
        tail_ = new_node;
    }
    size_++;
}

template<typename T, typename Allocator>
void MyList<T, Allocator>::clear() {
    Node* current = head_;
    while (current != nullptr) {
        Node* next = current->next;

        std::allocator_traits<NodeAllocator>::destroy(node_alloc_, current);
        std::allocator_traits<NodeAllocator>::deallocate(node_alloc_, current, 1);

        current = next;
    }
    head_ = tail_ = nullptr;
    size_ = 0;
}