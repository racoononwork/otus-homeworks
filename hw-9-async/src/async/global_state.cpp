#include "async/global_state.hpp"

#include "async/log_task.hpp"

namespace async::detail {
    GlobalState::GlobalState() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_logger_ = std::make_shared<spdlog::logger>("console", console_sink);
        console_logger_->set_level(spdlog::level::info);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("async_internal.log", true);
        file_logger_ = std::make_shared<spdlog::logger>("file", file_sink);
        file_logger_->set_level(spdlog::level::debug);

        spdlog::register_logger(console_logger_);
        spdlog::register_logger(file_logger_);
    }

    std::shared_ptr<GlobalState> GlobalState::instance() {
        std::call_once(GlobalState::instance_flag_, []() {
            global_state = std::make_shared<GlobalState>();
        });

        return global_state;
    }

    std::size_t GlobalState::create_context(std::size_t bulk_size) {
        // TODO
    }

    std::expected<ContextState*, GlobalErrors> GlobalState::find_context(std::size_t id) {
        std::lock_guard lock(this->ctx_mutex_);

        auto it = this->contexts_.find(id);
        if (it == this->contexts_.end()) { return std::make_unexpected(GlobalErrors::NotFound); }

        return &it->second;
    }
    ContextState GlobalState::take_and_erase_context(std::size_t id) {
        std::lock_guard lock(this->ctx_mutex_);

        auto it = this->contexts_.find(id);
        if (it == this->contexts_.end()) return;

        ContextState st =  std::move(it->second);
        this->contexts_.erase(it);

        return st;
    };

    void GlobalState::emplace_back(std::size_t bulk_size) {
        std::lock_guard lock(this->ctx_mutex_);
        std::size_t id = this->next_ctx_id_++;

        this->contexts_.emplace(id, ContextState{ id, bulk_size });
        this->console_logger()->info("connect: ctx={} bulk={}", id, bulk_size);

        return id;
    }

    bool GlobalState::has_contexts() {
        return !this->contexts_.empty();
    }

    // Worker lifecycle


    void GlobalState::ensure_workers_started() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true))
            return;

        auto& g = *this;

        log_thread_ = std::jthread([&g](std::stop_token st) {
            Task t = log_worker(g);
            t.h.resume();
            while (!st.stop_requested() && g.running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        file_thread1_ = std::jthread([&g](std::stop_token st) {
            Task t = file_worker(g, 1);
            t.h.resume();
            while (!st.stop_requested() && g.running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        file_thread2_ = std::jthread([&g](std::stop_token st) {
            Task t = file_worker(g, 2);
            t.h.resume();
            while (!st.stop_requested() && g.running_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }

    void GlobalState::request_shutdown() {
        if (!running_.exchange(false)) return;

        CommandBlock sentinel;
        sentinel.is_shutdown_marker = true;

        log_queue_.push(sentinel);
        file_queue_.push(sentinel);
        file_queue_.push(sentinel); // для второй файловой корутины
    }

    std::shared_ptr<spdlog::logger> GlobalState::console_logger() {}

    std::shared_ptr<spdlog::logger> GlobalState::file_logger() {}

    void GlobalState::start_workers_unsafe() {

    }
}
