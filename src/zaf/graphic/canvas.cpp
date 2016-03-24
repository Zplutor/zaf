#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const std::shared_ptr<Renderer>& renderer) :
	renderer_(renderer) {

	layer_ = renderer_->CreateLayer(Size());
	SaveState();
}


void Canvas::BeginPaint() {

	renderer_->Translate(absolute_rect_.position);

	Rect related_dirty_rect = absolute_paintable_rect_;
	related_dirty_rect.position.x -= absolute_rect_.position.x;
	related_dirty_rect.position.y -= absolute_rect_.position.y; 

	Layer::Parameters layer_param;
	layer_param.content_bounds = related_dirty_rect;
	renderer_->PushLayer(layer_param, layer_);
}


void Canvas::EndPaint() {

	renderer_->PopLayer();
}


void Canvas::SaveState() {

	auto new_state = std::make_shared<State>();
	new_state->brush = renderer_->CreateSolidColorBrush(Color::White);

	states_.push_back(new_state);
}


void Canvas::RestoreState() {

	if (! states_.empty()) {
		states_.pop_back();
	}
	else {
		ZAF_FAIL();
	}
}


std::shared_ptr<Canvas::State> Canvas::GetCurrentState() const {

	if (! states_.empty()) {
		return states_.back();
	}
	else {
		ZAF_FAIL();
		return nullptr;
	}
}

}