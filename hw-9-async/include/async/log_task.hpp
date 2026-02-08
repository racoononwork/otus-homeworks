#pragma once

#include <coroutine>
#include <iostream>
#include <fstream>
#include <ranges>

#include "async/global_state.hpp"

// coro task type
struct Task {
    struct promise_type {
        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> h;
    explicit Task(std::coroutine_handle<promise_type> h) : h(h) {}
    Task(Task&& other) noexcept : h(std::exchange(other.h, {})) {}
    ~Task() { if (h) h.destroy(); }
};

Task log_worker(async::detail::GlobalState& g) {
    auto logger = g.console_logger();
    logger->info("log_worker started");

    while (true) {
        auto block = co_await g.log_queue().pop();
        if (block.is_shutdown_marker) {
            logger->info("log_worker shutting down");
            co_return;
        }

        // bulk: cmd1, cmd2, ...
        fmt::print("bulk: ");
        for (auto&& c : block.commands | std::views::transform([](auto& s){ return s; })) {
            fmt::print("{} ", c);
        }
        fmt::print("\n");
    }
}

std::string make_filename(async::detail::TimePoint ts, int worker_id, std::uint64_t seq) {
    using namespace std::chrono;
    auto us = duration_cast<microseconds>(ts.time_since_epoch()).count();
    return fmt::format("bulk_{}_{}_{}.log", us, worker_id, seq);
}

Task file_worker(async::detail::GlobalState& g, int id) {
    auto logger = g.file_logger();
    logger->info("file_worker {} started", id);

    std::uint64_t seq = 0;

    while (true) {
        auto block = co_await g.file_queue().pop();
        if (block.is_shutdown_marker) {
            logger->info("file_worker {} shutting down", id);
            co_return;
        }

        const auto filename = make_filename(block.timestamp, id, seq++);

        std::ofstream out(filename);
        if (!out) {
            logger->error("failed to open {}", filename);
            continue;
        }
        for (auto&& cmd : block.commands)
            out << cmd << '\n';
    }
}
Task log_worker_coro(async::detail::GlobalState& g) {
    while (g.running) {
        auto block = co_await g.log_queue.pop();
        if (block.is_shutdown_marker)
            co_return;

        // Печать в консоль
        // Здесь можно использовать ranges:
        // for (const auto& cmd : block.commands | std::views::transform(...))
        std::cout << "bulk: ";
        for (const auto& cmd : block.commands) {
            std::cout << cmd << ' ';
        }
        std::cout << '\n';
    }
}

inline void start_workers_if_needed() {
    using namespace async::detail;
    auto& g = global_state();
    bool expected = false;
    if (!g.running.compare_exchange_strong(expected, true))
        return;

    g.log_thread = std::jthread([] {
        auto& g = global_state();
        Task t = log_worker_coro(g);
        // event loop: здесь просто spin до завершения корутины
        while (g.running) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });

    g.file_thread1 = std::jthread([] {
        auto& g = global_state();
        Task t = file_worker_coro(g, 1);
        while (g.running) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });

    g.file_thread2 = std::jthread([] {
        auto& g = global_state();
        Task t = file_worker_coro(g, 2);
        while (g.running) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
}


