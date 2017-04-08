#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/graphic/layer_parameters.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const Renderer& renderer) : renderer_(renderer) {

	layer_ = renderer_.CreateLayer(Size());
	SaveState();
}


void Canvas::BeginPaint() {

    clear_edge_absolute_rect_ = zaf::MakeClearEdgeForFill(absolute_rect_, ClearEdgeOption::Clear);
    renderer_.Transform(TransformMatrix::Translation(clear_edge_absolute_rect_.position));

	Rect clear_edge_absolute_paintable_rect = zaf::MakeClearEdgeForFill(absolute_paintable_rect_, ClearEdgeOption::Clear);
    clear_edge_absolute_paintable_rect.position.x -= clear_edge_absolute_rect_.position.x;
    clear_edge_absolute_paintable_rect.position.y -= clear_edge_absolute_rect_.position.y;

    renderer_.PushAxisAlignedClipping(clear_edge_absolute_paintable_rect, AntialiasMode::PerPrimitive);
}


void Canvas::EndPaint() {
    clear_edge_absolute_rect_ = Rect();
    renderer_.PopAxisAlignedClipping();
}


void Canvas::SaveState() {

	auto new_state = std::make_shared<State>();
	new_state->brush = renderer_.CreateSolidColorBrush(Color::White);

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