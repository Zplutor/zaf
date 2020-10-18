#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/application.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/geometry/rounded_rectangle_geometry.h>
#include <zaf/graphic/layer_parameters.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(const Renderer& renderer, const Rect& renderer_rect, const Rect& paintable_rect) :
    renderer_(renderer) {

    TransformLayer transformed_layer;
    transformed_layer.rect = renderer_rect;
    transformed_layer.paintable_rect = paintable_rect;
    transform_layers_.push(transformed_layer);

	SaveState();
}


void Canvas::PushTransformLayer(const Rect& rect, const Rect& paintable_rect) {

    const auto& current_transform_layer = transform_layers_.top();

    Rect new_transform_rect = rect;
    new_transform_rect.position.x += current_transform_layer.rect.position.x;
    new_transform_rect.position.y += current_transform_layer.rect.position.y;

    Rect new_paintable_rect = paintable_rect;
    new_paintable_rect.position.x += current_transform_layer.rect.position.x;
    new_paintable_rect.position.y += current_transform_layer.rect.position.y;
    new_paintable_rect.Intersect(current_transform_layer.paintable_rect);
    new_paintable_rect.Intersect(new_transform_rect);

    TransformLayer new_transform_layer;
    new_transform_layer.rect = new_transform_rect;
    new_transform_layer.paintable_rect = new_paintable_rect;

    transform_layers_.push(new_transform_layer);
}


void Canvas::PopTransformLayer() {
    transform_layers_.pop();
}


void Canvas::BeginPaint() {

    const auto& current_transform_layer = transform_layers_.top();

    auto aligned_transform_rect = Align(current_transform_layer.rect);
    renderer_.Transform(TransformMatrix::Translation(aligned_transform_rect.position));

    Rect paintable_rect = Align(current_transform_layer.paintable_rect);
    paintable_rect.position.x -= aligned_transform_rect.position.x;
    paintable_rect.position.y -= aligned_transform_rect.position.y;

    renderer_.PushAxisAlignedClipping(paintable_rect, AntialiasMode::PerPrimitive);

    aligned_transform_offset_.x = 
        current_transform_layer.rect.position.x - aligned_transform_rect.position.x;

    aligned_transform_offset_.y =
        current_transform_layer.rect.position.y - aligned_transform_rect.position.y;
}


void Canvas::EndPaint() {
    renderer_.PopAxisAlignedClipping();
    renderer_.Transform(TransformMatrix::Identity);
    aligned_transform_offset_ = {};
}


void Canvas::PushClippingRect(const Rect& rect) {

    auto absolute_clipping_rect = AlignWithOffset(rect);

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
	new_state->brush = renderer_.CreateSolidColorBrush(Color::White());

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


void Canvas::Clear() {
    renderer_.Clear();
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point, float stroke_width) {
    auto state = GetCurrentState();
    renderer_.DrawLine(
        AlignLineWithOffset(from_point, stroke_width),
        AlignLineWithOffset(to_point, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawRectangle(const Rect& rect) {
    auto state = GetCurrentState();
    renderer_.DrawRectangle(AlignWithOffset(rect), state->brush);
}


void Canvas::DrawRectangleFrame(const Rect& rect, float stroke_width) {
    auto state = GetCurrentState();
    renderer_.DrawRectangleFrame(
        AlignLineWithOffset(rect, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect) {
    auto state = GetCurrentState();
    renderer_.DrawRoundedRectangle(AlignWithOffset(rounded_rect), state->brush);
}


void Canvas::DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width) {
    auto state = GetCurrentState();
    renderer_.DrawRoundedRectangleFrame(
        AlignLineWithOffset(rounded_rect, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawEllipse(const Ellipse& ellipse) {
    auto state = GetCurrentState();
    renderer_.DrawEllipse(AlignWithOffset(ellipse), state->brush);
}


void Canvas::DrawEllipseFrame(const Ellipse& ellipse, float stroke_width) {
    auto state = GetCurrentState();
    renderer_.DrawEllipseFrame(
        AlignLineWithOffset(ellipse, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawGeometry(const Geometry& geometry) {
    renderer_.DrawGeometry(geometry, GetCurrentState()->brush, Brush());
}


void Canvas::DrawGeometryFrame(const Geometry& geometry, float stroke_width) {

    Geometry drew_geometry;

    //The geometry is not aligned for line, we need to do it by setting a new transform here.
    float offset = AlignmentOffsetForLine(stroke_width);
    if (offset != 0) {
        drew_geometry = GetGraphicFactory().CreateTransformedGeometry(
            geometry, 
            TransformMatrix::Translation(Point(offset, offset)));
    }
    else {
        drew_geometry = geometry;
    }

    auto state = GetCurrentState();
    renderer_.DrawGeometryFrame(drew_geometry, state->brush, stroke_width, state->stroke);
}


void Canvas::DrawTextFormat(
    const std::wstring& text, 
    const TextFormat& text_format, 
    const Rect& rect) {

    renderer_.DrawTextFormat(text, text_format, rect, GetCurrentState()->brush);
}


void Canvas::DrawTextLayout(const TextLayout& text_layout, const Point& position) {
    renderer_.DrawTextLayout(text_layout, position, GetCurrentState()->brush);
}


void Canvas::DrawBitmap(
    const RenderBitmap& bitmap,
    const Rect& destination_rect, 
    const DrawImageOptions& options) {

    renderer_.DrawBitmap(
        bitmap, 
        AlignWithOffset(destination_rect), 
        options.Opacity(),
        options.InterpolationMode(),
        options.SourceRect());
}


PathGeometry Canvas::CreatePathGeometry() const {

    ID2D1PathGeometry* handle{};
    HRESULT result = 
        GetGraphicFactory().GetDirect2dFactoryHandle()->CreatePathGeometry(&handle);

    ZAF_THROW_IF_COM_ERROR(result);
    return PathGeometry(handle, aligned_transform_offset_);
}


RectangleGeometry Canvas::CreateRectangleGeometry(const Rect& rect) const {

    Rect aligned_rect = rect;
    aligned_rect.position.x += aligned_transform_offset_.x;
    aligned_rect.position.y += aligned_transform_offset_.y;
    aligned_rect = Align(aligned_rect);

    return GetGraphicFactory().CreateRectangleGeometry(aligned_rect);
}


RoundedRectangleGeometry Canvas::CreateRoundedRectangleGeometry(
    const RoundedRect& rounded_rect) const {

    RoundedRect aligned_rounded_rect = rounded_rect;
    aligned_rounded_rect.rect.position.x += aligned_transform_offset_.x;
    aligned_rounded_rect.rect.position.y += aligned_transform_offset_.y;
    aligned_rounded_rect = Align(aligned_rounded_rect);

    return GetGraphicFactory().CreateRoundedRectangleGeometry(aligned_rounded_rect);
}


Point Canvas::AddOffset(const Point& point) const {
    return Point(point.x + aligned_transform_offset_.x, point.y + aligned_transform_offset_.y);
}

Rect Canvas::AddOffset(const Rect& rect) const {
    return Rect(AddOffset(rect.position), rect.size);
}

RoundedRect Canvas::AddOffset(const RoundedRect& rounded_rect) const {
    return RoundedRect(
        Rect(AddOffset(rounded_rect.rect.position), rounded_rect.rect.size), 
        rounded_rect.x_radius, 
        rounded_rect.y_radius);
}

Ellipse Canvas::AddOffset(const Ellipse& ellipse) const {
    return Ellipse(AddOffset(ellipse.position), ellipse.x_radius, ellipse.y_radius);
}

}