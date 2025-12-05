#pragma once
#include <cstdint>

namespace ecs {
    using Entity = std::uint32_t;          // простейший идентификатор
    constexpr Entity null_entity = 0;
}
