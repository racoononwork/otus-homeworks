#include "slint_vector_editor/ecs/registry.hpp"
#include "slint_vector_editor/systems/input_system.hpp"
#include "slint_vector_editor/systems/serialization_system.hpp"
#include "slint_vector_editor/view/slint_canvas_view.hpp"
#include "slint_vector_editor/controller/editor_controller.hpp"
#include <memory>
#include <string>

int main() {
    // Model
    auto registry = std::make_shared<ecs::Registry>();

    // View
    auto view = std::make_shared<SlintCanvasView>(registry);

    // Controller
    EditorController controller(registry, view);

    controller.run();

    return EXIT_SUCCESS;
}