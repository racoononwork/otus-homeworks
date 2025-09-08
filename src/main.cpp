#include <cassert>
#include <cstdlib>
#include <vector>

#include "include/data_structs.hpp"
#include "spdlog/spdlog.h"


auto main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[]) -> int {
    try {
        std::vector<IP> ip_pool = parse_ip_from_pipe();

        IP::sort_reverse_lex(ip_pool);

        auto print_all = [&](auto const &ips) {
            for (auto const &ip: ips) spdlog::info("{}", ip.to_string());
        };

        auto print_filtered = [&](auto const &ips, auto pred, std::string_view header = {}) {
            if (!header.empty()) spdlog::info("---- {} ----", header);
            for (auto const &ip: ips) {
                if (pred(ip)) spdlog::info("{}", ip.to_string());
            }
        };

        print_all(ip_pool);
        print_filtered(ip_pool, [](IP const &) { return true; }, "all"); // optional

        print_filtered(ip_pool, [](IP const &ip) {
            return ip.bytes()[0] == 1;
        }, "filter(1)");

        print_filtered(ip_pool, [](IP const &ip) {
            auto b = ip.bytes();
            return b[0] == 46 && b[1] == 70;
        }, "filter(46,70)");

        print_filtered(ip_pool, [](IP const &ip) {
            return std::ranges::any_of(ip.bytes(), [](int v) { return v == 46; });
        }, "filter_any(46)");
    } catch (const std::exception &e) {
        spdlog::error("exception: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
