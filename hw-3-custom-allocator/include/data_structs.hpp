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

