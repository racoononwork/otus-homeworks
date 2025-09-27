#pragma once

#include <algorithm>
#include <array>
#include <expected>
#include <ranges>
#include <compare>
#include <cstdint>
#include <string>
#include <vector>

enum ParseError {
    LengthError = 1,
    NotANumber
};

// header-only helpers
std::vector<std::string> split(const std::string &str, char d);

std::expected<int, ParseError> parse_int(std::string_view str);

std::expected<int, ParseError> parse_octet(std::string_view string_int);

// IP class
class IP {
public:
    IP() = delete;
    ~IP() = default;

    explicit IP(std::array<int, 4> ip);

    auto operator<=>(IP const&) const = default;

    template <std::ranges::random_access_range Cont>
    static void sort_reverse_lex(Cont& c) {
        std::ranges::sort(c,
            [](const IP& a, const IP& b) {
                return b.collapsed_ip_ < a.collapsed_ip_;
            });
    }

    [[nodiscard]] auto bytes() const noexcept { return split_ip_; }
    [[nodiscard]] std::string to_string() const;

private:
    std::array<int, 4> split_ip_{0, 0, 0, 0};
    uint32_t collapsed_ip_;
};

inline std::expected<IP, ParseError> make_ip(auto &&split_ip) {
    // Convert the lazy split_view into a concrete vector of strings.
    auto string_octets = split_ip | std::ranges::to<std::vector<std::string>>();

    if (string_octets.size() != 4) {
        return std::unexpected(ParseError::LengthError);
    }

    // Transform the vector of strings into a vector of expected integers.
    auto parsed_octets = string_octets
                       | std::views::transform(parse_octet)
                       | std::ranges::to<std::vector>();

    std::array<int, 4> _ip{0, 0, 0, 0};
    for (size_t index = 0; index < 4; ++index) {
        if (!parsed_octets[index].has_value()) {
            return std::unexpected(ParseError::NotANumber);
        }
        _ip[index] = parsed_octets[index].value();
    }

    return IP(_ip);
}


std::vector<IP> parse_ip_from_pipe();
