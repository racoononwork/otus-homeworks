// global_state.hpp
#pragma once
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "async_queue.hpp"

namespace async::detail {

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

struct CommandBlock {
    std::size_t context_id;
    std::vector<std::string> commands;
    TimePoint timestamp{};
    bool is_shutdown_marker = false;
};

struct ContextState {
    std::size_t id;
    std::size_t bulk_size;
    std::vector<std::string> current_commands;
    std::string tail;
};

enum GlobalErrors {
    NotFound = 1
};

class GlobalState {
public:
    static GlobalState& instance();

    // Контексты
    std::size_t create_context(std::size_t bulk_size);
    std::expected<ContextState*, GlobalErrors> find_context(std::size_t id);
    ContextState take_and_erase_context(std::size_t id);
    void emplace_back(std::size_t bulk_size);
    bool has_contexts() const;

    // Очереди
    AsyncQueue<CommandBlock>& log_queue()  { return log_queue_; }
    AsyncQueue<CommandBlock>& file_queue() { return file_queue_; }

    // Worker lifecycle
    void ensure_workers_started();
    void request_shutdown();

    // Логеры
    std::shared_ptr<spdlog::logger> console_logger();
    std::shared_ptr<spdlog::logger> file_logger();

private:
    GlobalState();
    void start_workers_unsafe();

private:
    static std::once_flag instance_flag_;

    mutable std::mutex ctx_mutex_;
    std::unordered_map<std::size_t, ContextState> contexts_;
    std::size_t next_ctx_id_ = 0;

    AsyncQueue<CommandBlock> log_queue_;
    AsyncQueue<CommandBlock> file_queue_;

    std::atomic<bool> running_{false};

    std::jthread log_thread_;
    std::jthread file_thread1_;
    std::jthread file_thread2_;

    std::shared_ptr<spdlog::logger> console_logger_;
    std::shared_ptr<spdlog::logger> file_logger_;
};
} // namespace async::detail
