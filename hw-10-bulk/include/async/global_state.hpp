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
#include <expected>

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
    GlobalState& operator=(const GlobalState &) = delete;
    static GlobalState& instance();

    std::size_t create_context(std::size_t bulk_size);
    std::expected<ContextState*, GlobalErrors> find_context(std::size_t id);
    std::expected<ContextState, GlobalErrors> take_and_erase_context(std::size_t id);
    std::size_t emplace_back(std::size_t bulk_size);

    bool has_contexts() const;

    AsyncQueue<CommandBlock>& log_queue()  { return log_queue_; }
    AsyncQueue<CommandBlock>& file_queue() { return file_queue_; }

    void ensure_workers_started();
    void request_shutdown();

    std::shared_ptr<spdlog::logger> console_logger();
    std::shared_ptr<spdlog::logger> file_logger();

    inline std::atomic<bool>& running()  { return running_; }
private:
    GlobalState();

private:
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
