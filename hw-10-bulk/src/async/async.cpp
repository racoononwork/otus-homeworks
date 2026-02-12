
#include "join_server/async.h"
#include "join_server/global_state.hpp"

#include <string>
#include <ranges>
#include <string_view>

async::handle_t async::connect(std::size_t bulk_size) {
    using namespace async::detail;
    auto& g = GlobalState::instance();
    g.ensure_workers_started();

    return new std::size_t(g.emplace_back(bulk_size));
}

void async::receive(handle_t h, const char* data, std::size_t size) {
    using namespace async::detail;
    auto& g = GlobalState::instance();
    auto id = *reinterpret_cast<std::size_t*>(h);

    auto result_st = g.find_context(id);
    if (!result_st.has_value()) { return; }

    ContextState* st = result_st.value();

    std::string_view chunk{ data, size };
    std::string buffer;
    buffer.reserve(st->tail.size() + chunk.size());
    buffer.append(st->tail);
    buffer.append(chunk);

    // TODO: check if it can be bottle neck with -O2 or not
    auto splitted = buffer | std::views::split('\n')
                           | std::views::transform([](auto&& rng) {
                                 return std::string(std::ranges::begin(rng), std::ranges::end(rng));
                             });

    std::vector<std::string> cmds;
    for (auto&& s : splitted) cmds.push_back(std::move(s));

    if (!buffer.empty() && buffer.back() != '\n') {
        st->tail = std::move(cmds.back());
        cmds.pop_back();
    } else {
        st->tail.clear();
    }

    for (auto& cmd : cmds | std::views::filter([](auto& s){ return !s.empty(); })) {
        st->current_commands.push_back(std::move(cmd));
        if (st->current_commands.size() == st->bulk_size) {
            CommandBlock block;
            block.context_id = st->id;
            block.commands = std::move(st->current_commands);
            block.timestamp = Clock::now();

            g.log_queue().push(block);
            g.file_queue().push(block);

            st->current_commands.clear();
        }
    }
}


void async::disconnect(handle_t h) {
    using namespace async::detail;
    auto& g = GlobalState::instance();
    auto id = *reinterpret_cast<std::size_t*>(h);

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
