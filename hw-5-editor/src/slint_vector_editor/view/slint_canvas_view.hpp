#pragma once

#include "editor.h" // Generated Slint header
#include "slint_vector_editor/ecs/registry.hpp"
#include "slint_vector_editor/utils/signal.hpp"

#include <memory>
#include <vector>

class SlintCanvasView {
public:
    core::Signal<> on_new_document;
    core::Signal<> on_open_dialog;
    core::Signal<> on_save_dialog;
    core::Signal<> on_add_line;
    core::Signal<> on_add_rect;

    explicit SlintCanvasView(std::shared_ptr<ecs::Registry> reg) 
        : registry(reg),
          window(MainWindow::create()) {
        setup_callbacks();
    }

    void show() {
        window->show();
        window->run();
    }

    void refresh() {
        if (!registry) return;

        std::vector<VisualShape> visual_shapes;
        for (const auto& entity : registry->view()) {
            VisualShape vs;
            vs.x = entity.transform.x;
            vs.y = entity.transform.y;
            vs.w = entity.transform.width;
            vs.h = entity.transform.height;
            vs.is_rect = (entity.shape.type == ShapeType::Rectangle);
            visual_shapes.push_back(vs);
        }

        auto model = std::make_shared<slint::VectorModel<VisualShape>>(visual_shapes);
        window->set_shapes_model(model);
    }

    void set_registry(std::shared_ptr<ecs::Registry> new_reg) {
        registry = new_reg;
        refresh();
    }

private:
    std::shared_ptr<ecs::Registry> registry;
    slint::ComponentHandle<MainWindow> window;

    void setup_callbacks() {
        window->on_new_doc([this]() { on_new_document.emit(); });
        window->on_open_doc([this]() { on_open_dialog.emit(); });
        window->on_save_doc([this]() { on_save_dialog.emit(); });
        window->on_add_line([this]() { on_add_line.emit(); });
        window->on_add_rect([this]() { on_add_rect.emit(); });
    }
};