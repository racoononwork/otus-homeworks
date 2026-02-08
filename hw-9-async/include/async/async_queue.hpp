#pragma once

#include <deque>
#include <mutex>
#include <optional>
#include <coroutine>
#include <condition_variable>


template <typename T>
class AsyncQueue {
public:
    struct PopAwaiter {
        AsyncQueue* q;
        std::optional<T> result;

        bool await_ready() {
            std::lock_guard lock(q->mtx_);

            if (!q->queue_.empty()) {
                result = std::move(q->queue_.front());

                q->queue_.pop_front();

                return true;
            }
            return false;
        }

        void await_suspend(std::coroutine_handle<> h) {
            std::lock_guard lock(q->mtx_);
            waiters_.push_back(h);
        }

        T await_resume() {
            return std::move(*result);
        }

    private:
        std::vector<std::coroutine_handle<>> waiters_;
    };

    void push(T value) {
        std::coroutine_handle<> waiter;
        {
            std::lock_guard lock(mtx_);
            if (!waiters_.empty()) {
                waiter = waiters_.back();
                waiters_.pop_back();

                queue_.push_back(std::move(value));
            } else {
                queue_.push_back(std::move(value));

                return;
            }
        }
        if (waiter) waiter.resume();
    }

    PopAwaiter pop() { return PopAwaiter{ this }; }

private:
    std::mutex mtx_;
    std::deque<T> queue_;
    std::vector<std::coroutine_handle<>> waiters_;
};
