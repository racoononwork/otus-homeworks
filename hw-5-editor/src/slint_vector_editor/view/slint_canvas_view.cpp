#include <slint_vector_editor/view/slint_canvas_view.hpp>
#include <editor.h>   

SlintCanvasView::SlintCanvasView(std::shared_ptr<ecs::Registry> reg)
    : reg_(std::move(reg))
{
    set_component(slint::load_component_from_path("editor.slint"));
    auto canvas = this->get_canvas();
    render_ = std::make_unique<RenderSystem>(*reg_);

    for (auto& [e, _] : reg_->view<Renderable>()) {
        auto* r = reg_->get<Renderable>(e);
        r->canvas = canvas;
    }

    this->on_new_document  = [this](){ /* … */ };
    this->on_open_dialog   = [this](){ /* … */ };
    this->on_save_dialog   = [this](){ /* … */ };
    this->on_add_line      = [this](){ /* … */ };
    this->on_add_rect      = [this](){ /* … */ };
}

void SlintCanvasView::refresh() {
    render_->draw();
}
