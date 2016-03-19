#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const std::shared_ptr<Renderer>& renderer) :
	renderer_(renderer),
	clear_edge_option_(ClearEdgeOption::None) {

	layer_ = renderer_->CreateLayer(Size());
	solid_color_brush_ = renderer_->CreateSolidColorBrush(Color::Black);
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


}