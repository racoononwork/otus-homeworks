#include "include/data_structs.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <ranges>

#include <spdlog/spdlog.h>

std::vector<std::string> split(const std::string &str, char d) {
    return str | std::views::split(d) | std::ranges::to<std::vector<std::string> >();
}

std::expected<int, ParseError> parse_int(std::string_view str) {
    int number = 0;
    std::stringstream ss(str.data());
    ss >> number;

    if (ss.fail() || !ss.eof()) {
        spdlog::error("Conversion failed or partial conversion occurred.");
        return std::unexpected(ParseError::NotANumber);
    }

    spdlog::debug("Converted number: {}", str);
    return number;
}

std::expected<int, ParseError> parse_octet(std::string_view string_int) {
    if (string_int.empty())
        return std::unexpected(ParseError::NotANumber);

    auto parsed_int = parse_int(string_int).or_else([]([[maybe_unused]] auto r) { return std::expected<int, ParseError>(-1); });
    if (!parsed_int.has_value()) return std::unexpected(ParseError::NotANumber);

    auto &&value = parsed_int.value();
    if (value < 0 || value > 255) return std::unexpected(ParseError::NotANumber);

    return value;
}

// IP implementation
IP::IP(std::array<int, 4> ip): split_ip_{ip}, collapsed_ip_(0) {
    collapsed_ip_ = (static_cast<std::uint32_t>(split_ip_[3])) |
                (static_cast<std::uint32_t>(split_ip_[2]) << 8) |
                (static_cast<std::uint32_t>(split_ip_[1]) << 16) |
                (static_cast<std::uint32_t>(split_ip_[0]) << 24);
}

std::string IP::to_string() const {
    std::ostringstream oss;
    oss << split_ip_[0] << '.' << split_ip_[1] << '.' << split_ip_[2] << '.' << split_ip_[3];
    return oss.str();
}


// parse_ip_from_pipe implementation
std::vector<IP> parse_ip_from_pipe() {
    std::vector<IP> ip_pool;

    for (std::string line; std::getline(std::cin, line);) {
        auto tabs = split(line, '\t');

        if (tabs.begin() == tabs.end()) continue;
        const std::string ip_str = tabs.front();

        auto maybe_ip = make_ip(ip_str | std::views::split('.'));

        if (!maybe_ip) {
            spdlog::warn("failed parsing IP: {}", ip_str);
            continue;
        }

        ip_pool.push_back(maybe_ip.value());
    }

    return ip_pool;
}
