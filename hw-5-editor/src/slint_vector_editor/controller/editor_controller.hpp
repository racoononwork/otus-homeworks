#pragma once

#include "slint_vector_editor/ecs/registry.hpp"
#include "slint_vector_editor/systems/input_system.hpp"
#include "slint_vector_editor/systems/serialization_system.hpp"
#include "slint_vector_editor/view/slint_canvas_view.hpp"

#include <memory>
#include <string>

class EditorController {
public:
    EditorController(std::shared_ptr<ecs::Registry> registry, std::shared_ptr<SlintCanvasView> view)
        : registry_(std::move(registry))
        , view_(std::move(view))
        , input_(*registry_)
        , storage_(*registry_)
    {
        setup_connections();
    }

    void run() {
        view_->show();
    }

private:
    std::shared_ptr<ecs::Registry> registry_;
    std::shared_ptr<SlintCanvasView> view_;
    InputSystem input_;
    SerializationSystem storage_;

    void setup_connections() {
        view_->on_new_document.connect([this]() {
            registry_->clear(); 
            view_->refresh();
        });

        view_->on_open_dialog.connect([this]() {
            std::string path = "example.vec";
            storage_.load(path);
            view_->refresh();
        });

        view_->on_save_dialog.connect([this]() {
            std::string path = "example.vec";
            storage_.save(path);
        });

        view_->on_add_line.connect([this]() {
            input_.addLine(10.f, 10.f, 200.f, 150.f);
            view_->refresh();
        });

        view_->on_add_rect.connect([this]() {
            input_.addRect(50.f, 50.f, 120.f, 80.f);
            view_->refresh();
        });
    }
};