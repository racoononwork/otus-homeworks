#pragma once

#include <deque>
#include <mutex>
#include <optional>
#include <coroutine>
#include <vector>

namespace async::detail {

    template <typename T>
    struct Generator {
        struct promise_type;
        using handle_t = std::coroutine_handle<promise_type>;

        struct promise_type {
            T value;
            std::suspend_always yield_value(T val) {
                value = std::move(val);
                return {};
            }
            Generator get_return_object() {
                return Generator{handle_t::from_promise(*this)};
            }
            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() {}
        };

        handle_t h;
        explicit Generator(handle_t h) : h(h) {}
        Generator(const Generator&) = delete;
        ~Generator() { if (h) h.destroy(); }

        bool next() {
            h.resume();
            return !h.done();
        }

        T get() {
            return std::move(h.promise().value);
        }
    };

    template <typename T>
    class AsyncQueue {
    public:
        void push(T value) {
            {
                std::lock_guard lock(mutex_);
                queue_.push_back(std::move(value));
            }
            cv_.notify_one();
        }

        T pop() {
            std::unique_lock lock(mutex_);
            cv_.wait(lock, [this] { return !queue_.empty(); });
            T item = std::move(queue_.front());
            queue_.pop_front();
            return item;
        }

        bool try_pop(T& item) {
            std::unique_lock lock(mutex_);
            if (queue_.empty()) return false;
            item = std::move(queue_.front());
            queue_.pop_front();
            return true;
        }

    private:
        mutable std::mutex mutex_;
        std::deque<T> queue_;
        std::condition_variable cv_;
    };


} // namespace async::detail
