#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/layer_parameters.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const Renderer& renderer, const Rect& renderer_rect, const Rect& paintable_rect) :
    renderer_(renderer) {

    TransformedLayer transformed_layer;
    transformed_layer.rect = renderer_rect;
    transformed_layer.paintable_rect = paintable_rect;
    transformed_layers_.push(transformed_layer);

	SaveState();
}


void Canvas::PushTransformLayer(const Rect& rect, const Rect& paintable_rect) {

    const auto& current_transformed_layer = transformed_layers_.top();

    Rect new_transformed_rect = rect;
    new_transformed_rect.position.x += current_transformed_layer.rect.position.x;
    new_transformed_rect.position.y += current_transformed_layer.rect.position.y;

    Rect new_paintable_rect = paintable_rect;
    new_paintable_rect.position.x += current_transformed_layer.rect.position.x;
    new_paintable_rect.position.y += current_transformed_layer.rect.position.y;
    new_paintable_rect.Intersect(current_transformed_layer.paintable_rect);
    new_paintable_rect.Intersect(new_transformed_rect);

    TransformedLayer new_transformed_layer;
    new_transformed_layer.rect = new_transformed_rect;
    new_transformed_layer.paintable_rect = new_paintable_rect;

    transformed_layers_.push(new_transformed_layer);
}


void Canvas::PopTransformLayer() {
    transformed_layers_.pop();
}


void Canvas::BeginPaint() {

    const auto& current_transformed_layer = transformed_layers_.top();

    auto clear_edge_transformed_rect = zaf::MakeClearEdgeForFill(current_transformed_layer.rect, ClearEdgeOption::Clear);
    renderer_.Transform(TransformMatrix::Translation(clear_edge_transformed_rect.position));

    Rect paintable_rect = zaf::MakeClearEdgeForFill(current_transformed_layer.paintable_rect, ClearEdgeOption::Clear);
    paintable_rect.position.x -= clear_edge_transformed_rect.position.x;
    paintable_rect.position.y -= clear_edge_transformed_rect.position.y;

    renderer_.PushAxisAlignedClipping(paintable_rect, AntialiasMode::PerPrimitive);

    transformed_rect_offset_ = current_transformed_layer.rect.position;
    transformed_rect_clear_edge_offset_ = clear_edge_transformed_rect.position;
}


void Canvas::EndPaint() {
    renderer_.PopAxisAlignedClipping();
}


void Canvas::PushClippingRect(const Rect& rect) {

    auto absolute_clipping_rect = MakeClearEdgeForFill(rect);

    GetCurrentState()->clipping_rects.push_back(absolute_clipping_rect);
    renderer_.PushAxisAlignedClipping(absolute_clipping_rect, zaf::AntialiasMode::PerPrimitive);
}


void Canvas::PopClippingRect() {

    GetCurrentState()->clipping_rects.pop_back();
    renderer_.PopAxisAlignedClipping();
}


void Canvas::SaveState() {

    if (! states_.empty()) {

        auto current_state = states_.back();
        CancelState(current_state);
    }

	auto new_state = std::make_shared<State>();
	new_state->brush = renderer_.CreateSolidColorBrush(Color::White);

	states_.push_back(new_state);
    ApplyState(new_state);
}


void Canvas::RestoreState() {

	if (! states_.empty()) {

        auto current_state = states_.back();
        CancelState(current_state);
		states_.pop_back();

        if (! states_.empty()) {
            current_state = states_.back();
            ApplyState(current_state);
        }
	}
	else {
		ZAF_FAIL();
	}
}


void Canvas::ApplyState(const std::shared_ptr<State>& state) {

    for (const auto& each_rect : state->clipping_rects) {
        renderer_.PushAxisAlignedClipping(each_rect, zaf::AntialiasMode::PerPrimitive);
    }
}


void Canvas::CancelState(const std::shared_ptr<State>& state) {

    for (const auto& each_rect : state->clipping_rects) {
        renderer_.PopAxisAlignedClipping();
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


Point Canvas::MakeClearEdgeForLine(const Point& point, float stroke_width) const {

    auto new_point = zaf::MakeClearEdgeForLine(
        AddAbsoluteOffset(point),
        stroke_width,
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_point);
    return new_point;
}

Point Canvas::MakeClearEdgeForFill(const Point& point) const {

    auto new_point = zaf::MakeClearEdgeForFill(
        AddAbsoluteOffset(point),
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_point);
    return new_point;
}


Rect Canvas::MakeClearEdgeForLine(const Rect& rect, float stroke_width) const {

    auto new_rect =  zaf::MakeClearEdgeForLine(
        AddAbsoluteOffset(rect),
        stroke_width, 
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_rect);
    return new_rect;
}

Rect Canvas::MakeClearEdgeForFill(const Rect& rect) const {

    auto new_rect = zaf::MakeClearEdgeForFill(
        AddAbsoluteOffset(rect),
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_rect);
    return new_rect;
}


RoundedRect Canvas::MakeClearEdgeForLine(const RoundedRect& rounded_rect, float stroke_width) const {

    auto new_rounded_rect = zaf::MakeClearEdgeForLine(
        AddAbsoluteOffset(rounded_rect), 
        stroke_width, 
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_rounded_rect);
    return new_rounded_rect;
}

RoundedRect Canvas::MakeClearEdgeForFill(const RoundedRect& rounded_rect) const {

    auto new_rounded_rect = zaf::MakeClearEdgeForFill(
        AddAbsoluteOffset(rounded_rect),
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_rounded_rect);
    return new_rounded_rect;
}


Ellipse Canvas::MakeClearEdgeForLine(const Ellipse& ellipse, float stroke_width) const {

    auto new_ellipse = zaf::MakeClearEdgeForLine(
        AddAbsoluteOffset(ellipse),
        stroke_width, 
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_ellipse);
    return new_ellipse;
}

Ellipse Canvas::MakeClearEdgeForFill(const Ellipse& ellipse) const {

    auto new_ellipse = zaf::MakeClearEdgeForFill(
        AddAbsoluteOffset(ellipse),
        GetCurrentState()->clear_edge_option);

    RemoveClearEdgeAbsoluteOffset(new_ellipse);
    return new_ellipse;
}

}