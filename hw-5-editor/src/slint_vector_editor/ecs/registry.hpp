#pragma once
#include "slint_vector_editor/components/components.hpp"
#include <vector>
#include <optional>
#include <algorithm>
#include <memory>

namespace ecs {

class Registry {
public:
    struct Entity {
        uint32_t id;
        TransformComponent transform;
        ShapeComponent shape;
        bool active = false;
    };

    uint32_t createEntity(ShapeType type, float x, float y, float w, float h) {
        static uint32_t counter = 0;
        uint32_t newId = ++counter;
        
        entities.push_back({
            newId,
            {x, y, w, h},
            {type, 0xFF0000FF},
            true
        });
        return newId;
    }

    const std::vector<Entity>& view() const {
        return entities;
    }

    void clear() {
        entities.clear();
    }

private:
    std::vector<Entity> entities;
};

} // namespace ecs