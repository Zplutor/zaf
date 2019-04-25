#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/layer_parameters.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const Renderer& renderer, const Rect& renderer_rect) : 
    renderer_(renderer) {

    transformed_rects_.push(renderer_rect);
	SaveState();
}


void Canvas::PushTransformRect(const Rect& rect) {

    const auto& current_transformed_rect = transformed_rects_.top();

    Rect new_transformed_rect = rect;
    new_transformed_rect.position.x += current_transformed_rect.position.x;
    new_transformed_rect.position.y += current_transformed_rect.position.y;

    new_transformed_rect.Intersect(current_transformed_rect);
    transformed_rects_.push(new_transformed_rect);
}


void Canvas::PopTransformRect() {
    transformed_rects_.pop();
}


void Canvas::BeginPaint() {

    const auto& current_transformed_rect = transformed_rects_.top();

    current_transformed_rect_ = zaf::MakeClearEdgeForFill(current_transformed_rect, ClearEdgeOption::Clear);
    renderer_.Transform(TransformMatrix::Translation(current_transformed_rect_.position));

    Rect clipping_rect(Point(), current_transformed_rect_.size);
    renderer_.PushAxisAlignedClipping(clipping_rect, AntialiasMode::PerPrimitive);
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