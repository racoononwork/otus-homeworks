#include "async/async.h"
#include "async/global_state.hpp"

#include <string>
#include <ranges>
#include <string_view>

async::handle_t async::connect(std::size_t bulk_size) {
    using namespace async::detail;
    auto &g = GlobalState::instance();
    g.ensure_workers_started();

    return new std::size_t(g.emplace_back(bulk_size));
}

void async::receive(handle_t h, const char *data, std::size_t size) {
    using namespace async::detail;
    auto &g = GlobalState::instance();
    auto id = *reinterpret_cast<std::size_t *>(h);

    auto result_st = g.find_context(id);
    if (!result_st.has_value()) { return; }

    ContextState *st = result_st.value();

    std::string_view chunk{data, size};
    std::string buffer;
    buffer.reserve(st->tail.size() + chunk.size());
    buffer.append(st->tail);
    buffer.append(chunk);

    // TODO: check if it can be bottle neck with -O2 or not
    auto splitted = buffer | std::views::split('\n')
                    | std::views::transform([](auto &&rng) {
                        return std::string(std::ranges::begin(rng), std::ranges::end(rng));
                    });

    std::vector<std::string> cmds;
    for (auto &&s: splitted) cmds.push_back(std::move(s));

    auto lines = std::vector<std::string>();
    std::string_view view(buffer.data(), size);

    while (!view.empty()) {
        auto pos = view.find("\n");
        if (pos != std::string_view::npos) {
            lines.emplace_back(std::string(view.substr(0, pos)));
            view.remove_prefix(pos + 1);
        } else {
            st->tail.assign(view.data(), view.size());
            break;
        }
    }

    for (auto &cmd: cmds) {
        if (!cmd.empty()) {
            st->current_commands.push_back(std::move(cmd));
            if (st->current_commands.size() == st->bulk_size) {
                // push block, clear
                CommandBlock block{st->id, std::move(st->current_commands), Clock::now()};
                g.log_queue().push(block);
                g.file_queue().push(block);
                st->current_commands.clear();
            }
        }
    }
}


void async::disconnect(handle_t h) {
    using namespace async::detail;
    auto &g = GlobalState::instance();
    auto id = *reinterpret_cast<std::size_t *>(h);

    ContextState st;
    if (auto st_res = g.take_and_erase_context(id); !st_res.has_value()) { return; } else {
        st = st_res.value();
    }

    if (!st.tail.empty())
        st.current_commands.push_back(std::move(st.tail));

    if (!st.current_commands.empty()) {
        CommandBlock block;
        block.context_id = st.id;
        block.commands = std::move(st.current_commands);
        block.timestamp = Clock::now();

        g.log_queue().push(block);
        g.file_queue().push(block);
    }

    if (!g.has_contexts()) {
        g.request_shutdown();
    }
}
