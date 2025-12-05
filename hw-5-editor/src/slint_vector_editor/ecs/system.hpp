#pragma once

#include <slint_vector_editor/registry.hpp>

namespace ecs {

class System {
public:
    explicit System(Registry& reg) : registry_(reg) {}
    virtual ~System() = default;

    virtual void update() {}

protected:
    Registry& registry_;
};

} // namespace ecs
