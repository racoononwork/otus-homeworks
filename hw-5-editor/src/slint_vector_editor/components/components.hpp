#pragma once
#include <cstdint>

enum class ShapeType {
    Rectangle,
    Line
};

struct TransformComponent {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f; 
    float height = 0.0f; 
};

struct ShapeComponent {
    ShapeType type;
    uint32_t color = 0xFF0000FF; // RGBA
};

struct IDComponent {
    uint32_t id;
};