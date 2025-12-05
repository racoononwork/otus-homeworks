#pragma once

#include <slint_vector_editor/ecs/registry.hpp>
#include <slint_vector_editor/components/transform.hpp>
#include <slint_vector_editor/components/shape_data.hpp>
#include <slint_vector_editor/components/renderable.hpp>


class RenderSystem {
public:
    explicit RenderSystem(ecs::Registry& r) : reg_(r) {}

    void draw() {
        auto& trans = reg_.view<Transform>();
        auto& shape = reg_.view<ShapeData>();
        auto& rend  = reg_.view<Renderable>();

        for (auto& [e, rcomp] : rend) {
            auto* t = reg_.get<Transform>(e);
            auto* s = reg_.get<ShapeData>(e);
            if (!t || !s) continue;

            auto* canvas = rcomp.canvas.get();
            if (!canvas) continue;

            switch (s->kind) {
            case ShapeKind::Line: {
                const auto& d = std::get<LineData>(s->payload);
                canvas->draw_line(d.pts[0], d.pts[1],
                                  d.pts[2], d.pts[3],
                                  slint::Color::from_argb_u32(0xFF0000FF));
                break;
            }
            case ShapeKind::Rectangle: {
                const auto& d = std::get<RectData>(s->payload);
                canvas->draw_rect(d.rect[0], d.rect[1],
                                  d.rect[2], d.rect[3],
                                  slint::Color::from_argb_u32(0xFF00FF00));
                break;
            }
            }
        }
    }

private:
    ecs::Registry& reg_;
};
