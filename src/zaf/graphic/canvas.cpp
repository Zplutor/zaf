#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/layer_parameters.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const std::shared_ptr<Renderer>& renderer) :
	renderer_(renderer) {

	layer_ = renderer_->CreateLayer(Size());
	SaveState();
}


void Canvas::BeginPaint() {

	Rect adjusted_absolute_rect = MakeClearEdgeRectForFill(absolute_rect_, ClearEdgeOption::Clear);
	renderer_->Transform(TransformMatrix::Translation(adjusted_absolute_rect.position));

	Rect adjusted_absolute_paintable_rect = MakeClearEdgeRectForFill(absolute_paintable_rect_, ClearEdgeOption::Clear);
	adjusted_absolute_paintable_rect.position.x -= adjusted_absolute_rect.position.x;
	adjusted_absolute_paintable_rect.position.y -= adjusted_absolute_rect.position.y;

	LayerParameters layer_param;
	layer_param.content_bounds = adjusted_absolute_paintable_rect;
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