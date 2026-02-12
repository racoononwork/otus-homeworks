#include "join_server/global_state.hpp"
#include <format>
#include <iomanip>
#include <sstream>
#include <chrono>

namespace async::detail {
using Clock = std::chrono::system_clock;
    void log_worker(GlobalState& g) {
        std::shared_ptr<spdlog::logger> logger = g.console_logger();
        CommandBlock block;

        while (g.running()) {
            block = g.log_queue().pop();
            if (block.is_shutdown_marker) break;

            // ✅ Правильный формат "bulk: 0, 1, 2"
            std::string cmds;
            for (size_t i = 0; i < block.commands.size(); ++i) {
                if (i > 0) cmds += ", ";
                cmds += block.commands[i];
            }
            logger->info("bulk: {}", cmds);
        }
    }

void file_worker(GlobalState& g, int thread_id) {
    auto now = Clock::now();
    auto now_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&now_t);
    char timebuf[32];

    std::strftime(timebuf, sizeof(timebuf), "%H%M%S", &tm);
    std::string logger_name = std::format("file{}_{}", thread_id, timebuf);
    std::string fname = std::format("async_{}_t{}.log", timebuf, thread_id);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fname, true);
    auto logger = std::make_shared<spdlog::logger>(logger_name, file_sink);
    logger->set_level(spdlog::level::debug);

    if (!spdlog::get(logger_name)) {
        spdlog::register_logger(logger);
    }

    g.console_logger()->info("File worker {} → {}", thread_id, fname);

    CommandBlock block;
    while (g.running()) {
        block = g.file_queue().pop();
        if (block.is_shutdown_marker) {
            logger->info("{} shutdown", logger_name);
            break;
        }

        for (const auto& cmd : block.commands) {
            logger->debug("{}", cmd);
        }
    }
}



GlobalState& GlobalState::instance() {
    static GlobalState g;
    return g;
}

GlobalState::GlobalState() {
    // Console logger (info+)
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_logger_ = std::make_shared<spdlog::logger>("console", console_sink);
    console_logger_->set_level(spdlog::level::info);
    spdlog::register_logger(console_logger_);

    // File logger (debug+) для internal.log
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("async_internal.log", true);
    file_sink->set_level(spdlog::level::debug);
    file_logger_ = std::make_shared<spdlog::logger>("file", file_sink);
    file_logger_->set_level(spdlog::level::debug);
    spdlog::register_logger(file_logger_);
}


std::size_t GlobalState::create_context(std::size_t bulk_size) {
    return emplace_back(bulk_size);
}

std::expected<ContextState*, GlobalErrors> GlobalState::find_context(std::size_t id) {
    std::lock_guard lock(ctx_mutex_);
    auto it = contexts_.find(id);
    if (it == contexts_.end()) return std::unexpected(GlobalErrors::NotFound);
    return &it->second;
}

std::expected<ContextState, GlobalErrors> GlobalState::take_and_erase_context(std::size_t id) {
    std::lock_guard lock(ctx_mutex_);
    auto it = contexts_.find(id);
    if (it == contexts_.end()) return std::unexpected(GlobalErrors::NotFound);
    ContextState st = std::move(it->second);
    contexts_.erase(it);
    return st;
}

std::size_t GlobalState::emplace_back(std::size_t bulk_size) {
    std::lock_guard lock(ctx_mutex_);
    std::size_t id = next_ctx_id_++;
    contexts_.emplace(id, ContextState{id, bulk_size});
    console_logger_->info("connect: ctx={} bulk={}", id, bulk_size);
    return id;
}

bool GlobalState::has_contexts() const {
    std::lock_guard lock(ctx_mutex_);
    return !contexts_.empty();
}

void GlobalState::ensure_workers_started() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) return;

    this->console_logger()->info("Starting workers: log + file1 + file2");

    log_thread_ = std::jthread([this](std::stop_token st) {
        log_worker(*this);
    });

    file_thread1_ = std::jthread([this](std::stop_token st) {
        file_worker(*this, 1);
    });

    file_thread2_ = std::jthread([this](std::stop_token st) {
        file_worker(*this, 2);
    });
}


void GlobalState::request_shutdown() {
    if (!running_.exchange(false)) return;
    CommandBlock sentinel{0, {}, Clock::now(), true};
    log_queue_.push(sentinel);
    file_queue_.push(sentinel);
    file_queue_.push(sentinel);
}

std::shared_ptr<spdlog::logger> GlobalState::console_logger() {
    return console_logger_;
}

std::shared_ptr<spdlog::logger> GlobalState::file_logger() {
    return file_logger_;
}

} // namespace async::detail
