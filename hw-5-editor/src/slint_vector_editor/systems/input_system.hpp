#pragma once
#include "slint_vector_editor/ecs/registry.hpp"

class InputSystem {
public:
    explicit InputSystem(ecs::Registry& reg) : registry(reg) {}

    void addLine(float x1, float y1, float x2, float y2) {
        registry.createEntity(ShapeType::Line, x1, y1, x2 - x1, y2 - y1);
    }

    void addRect(float x, float y, float w, float h) {
        registry.createEntity(ShapeType::Rectangle, x, y, w, h);
    }

    void reset() {
        registry.clear();
    }

private:
    ecs::Registry& registry;
};