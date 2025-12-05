#include "slint_vector_editor/ecs/registry.hpp"
#include "slint_vector_editor/systems/input_system.hpp"
#include "slint_vector_editor/systems/serialization_system.hpp"
#include "slint_vector_editor/view/slint_canvas_view.hpp"
#include <memory>
#include <string>

int main() {
    auto registry = std::make_shared<ecs::Registry>();

    InputSystem          input(*registry);
    SerializationSystem  storage(*registry);

    SlintCanvasView view(registry);


    view.on_new_document.connect([&]() {
        registry->clear(); 
        view.refresh();
    });
    view.on_open_dialog.connect([&]() {
        std::string path = "example.vec";
        storage.load(path);
        view.refresh();
    });
    view.on_save_dialog.connect([&]() {
        std::string path = "example.vec";
        storage.save(path);
    });
    view.on_add_line.connect([&]() {
        input.addLine(10.f, 10.f, 200.f, 150.f);
        view.refresh();
    });
    view.on_add_rect.connect([&]() {
        input.addRect(50.f, 50.f, 120.f, 80.f);
    });
    view.on_add_rect.connect([&]() {
        view.refresh();
    });

    view.show();
    return EXIT_SUCCESS;
}