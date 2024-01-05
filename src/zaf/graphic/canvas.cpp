#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/geometry/internal/aligned_path_geometry.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/geometry/rounded_rectangle_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(zaf::Renderer& renderer) : renderer_(renderer) {

    //Add an initial state.
    internal::CanvasState initial_state;
    initial_state.brush = renderer.CreateSolidColorBrush(Color::White());
    states_.push(initial_state);
}


Canvas::~Canvas() {

}


CanvasRegionGuard Canvas::PushRegion(const Rect& region_rect, const Rect& paintable_rect) {

    auto new_region = CreateNewRegion(region_rect, paintable_rect);
    regions_.push(new_region);

    renderer_.Transform(TransformMatrix::Translation(new_region.aligned_rect.position));

    Rect clipping_rect = new_region.aligned_paintable_rect;
    clipping_rect.SubtractOffset(new_region.aligned_rect.position);
    auto clipping_guard = InnerPushClipping(clipping_rect);

    return CanvasRegionGuard{ this, std::move(clipping_guard) };
}


internal::CanvasRegion Canvas::CreateNewRegion(
    const Rect& region_rect,
    const Rect& paintable_rect) const {

    const internal::CanvasRegion* current_region{};
    if (!regions_.empty()) {
        current_region = &regions_.top();
    }

    internal::CanvasRegion new_region;
    new_region.rect = region_rect;
    if (current_region) {
        new_region.rect.AddOffset(current_region->rect.position);
    }
    new_region.aligned_rect = ToPixelAligned(new_region.rect, renderer_.GetDPI());

    new_region.paintable_rect = paintable_rect;
    if (current_region) {
        new_region.paintable_rect.AddOffset(current_region->rect.position);
        new_region.paintable_rect.Intersect(current_region->paintable_rect);
    }
    new_region.paintable_rect.Intersect(new_region.rect);
    new_region.aligned_paintable_rect = ToPixelAligned(new_region.paintable_rect, renderer_.GetDPI());
    return new_region;
}


void Canvas::PopRegion(CanvasClippingGuard&& clipping_guard) {

    ZAF_EXPECT(!regions_.empty());

    regions_.pop();

    //Destroy clipping guard to pop the clipping.
    {
        auto temp_clipping_guard = std::move(clipping_guard);
    }

    if (!regions_.empty()) {
        renderer_.Transform(TransformMatrix::Translation(regions_.top().aligned_rect.position));
    }
    else {
        renderer_.Transform(TransformMatrix::Identity);
    }
}


CanvasClippingGuard Canvas::PushClipping(const Rect& clipping_rect) {

    auto aligned_clipping_rect = AlignWithRegion(clipping_rect);
    return InnerPushClipping(aligned_clipping_rect);
}


CanvasClippingGuard Canvas::InnerPushClipping(const Rect& clipping_rect) {

    renderer_.PushAxisAlignedClipping(clipping_rect, zaf::AntialiasMode::PerPrimitive);
    return CanvasClippingGuard{ this, ++current_clipping_tag_ };
}


void Canvas::PopClipping(std::size_t tag) {

    //Detect mismatch push and pop.
    ZAF_EXPECT(tag == current_clipping_tag_);

    renderer_.PopAxisAlignedClipping();
    --current_clipping_tag_;
}


CanvasStateGuard Canvas::PushState() {

    //Copy current state as new state.
    auto new_state = states_.top();
    states_.push(new_state);
    return CanvasStateGuard{ this, ++current_state_tag_ };
}


void Canvas::PopState(std::size_t tag) {

    //Detect mismatch push and pop.
    ZAF_EXPECT(tag == current_state_tag_);

    states_.pop();
    --current_state_tag_;
}


void Canvas::SetBrush(const Brush& brush) {
    states_.top().brush = brush;
}


void Canvas::SetBrushWithColor(const Color& color) {
    auto brush = renderer_.CreateSolidColorBrush(color);
    SetBrush(brush);
}


void Canvas::SetStroke(const Stroke& stroke) {
    states_.top().stroke = stroke;
}


const internal::CanvasState& Canvas::CurrentState() const {
    return states_.top();
}


void Canvas::Clear() {
    renderer_.Clear();
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point, float stroke_width) {

    const auto& state = CurrentState();
    renderer_.DrawLine(
        AlignWithRegion(from_point, stroke_width),
        AlignWithRegion(to_point, stroke_width),
        state.brush,
        stroke_width,
        state.stroke);
}


void Canvas::DrawRectangle(const Rect& rect) {
    const auto& state = CurrentState();
    DrawRectangle(rect, state.brush);
}


void Canvas::DrawRectangle(const Rect& rect, const Color& color) {
    DrawRectangle(rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::DrawRectangle(const Rect& rect, const Brush& brush) {
    renderer_.DrawRectangle(AlignWithRegion(rect), brush);
}


void Canvas::DrawRectangleFrame(const Rect& rect, float stroke_width) {
    const auto& state = CurrentState();
    DrawRectangleFrame(rect, stroke_width, state.brush, state.stroke);
}


void Canvas::DrawRectangleFrame(const Rect& rect, float stroke_width, const Brush& brush) {
    const auto& state = CurrentState();
    DrawRectangleFrame(rect, stroke_width, brush, state.stroke);
}


void Canvas::DrawRectangleFrame(const Rect& rect, float stroke_width, const Stroke& stroke) {
    const auto& state = CurrentState();
    DrawRectangleFrame(rect, stroke_width, state.brush, stroke);
}


void Canvas::DrawRectangleFrame(
    const Rect& rect,
    float stroke_width,
    const Brush& brush,
    const Stroke& stroke) {

    renderer_.DrawRectangleFrame(AlignWithRegion(rect, stroke_width), brush, stroke_width, stroke);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect) {
    const auto& state = CurrentState();
    DrawRoundedRectangle(rounded_rect, state.brush);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect, const Color& color) {
    DrawRoundedRectangle(rounded_rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect, const Brush& brush) {
    renderer_.DrawRoundedRectangle(AlignWithRegion(rounded_rect), brush);
}


void Canvas::DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width) {
    const auto& state = CurrentState();
    DrawRoundedRectangleFrame(rounded_rect, stroke_width, state.brush, state.stroke);
}


void Canvas::DrawRoundedRectangleFrame(
    const RoundedRect& rounded_rect,
    float stroke_width,
    const Brush& brush) {

    const auto& state = CurrentState();
    DrawRoundedRectangleFrame(rounded_rect, stroke_width, brush, state.stroke);
}


void Canvas::DrawRoundedRectangleFrame(
    const RoundedRect& rounded_rect,
    float stroke_width,
    const Stroke& stroke) {

    const auto& state = CurrentState();
    DrawRoundedRectangleFrame(rounded_rect, stroke_width, state.brush, stroke);
}


void Canvas::DrawRoundedRectangleFrame(
    const RoundedRect& rounded_rect,
    float stroke_width,
    const Brush& brush,
    const Stroke& stroke) {

    renderer_.DrawRoundedRectangleFrame(
        AlignWithRegion(rounded_rect, stroke_width),
        brush,
        stroke_width,
        stroke);
}


void Canvas::DrawEllipse(const Ellipse& ellipse) {
    const auto& state = CurrentState();
    renderer_.DrawEllipse(ellipse, state.brush);
}


void Canvas::DrawEllipse(const Ellipse& ellipse, const Brush& brush) {
    renderer_.DrawEllipse(AlignWithRegion(ellipse), brush);
}


void Canvas::DrawEllipseFrame(const Ellipse& ellipse, float stroke_width) {
    const auto& state = CurrentState();
    DrawEllipseFrame(ellipse, stroke_width, state.brush, state.stroke);
}


void Canvas::DrawEllipseFrame(const Ellipse& ellipse, float stroke_width, const Brush& brush) {
    const auto& state = CurrentState();
    DrawEllipseFrame(ellipse, stroke_width, brush, state.stroke);
}


void Canvas::DrawEllipseFrame(const Ellipse& ellipse, float stroke_width, const Stroke& stroke) {
    const auto& state = CurrentState();
    DrawEllipseFrame(ellipse, stroke_width, state.brush, stroke);
}


void Canvas::DrawEllipseFrame(
    const Ellipse& ellipse,
    float stroke_width,
    const Brush& brush,
    const Stroke& stroke) {

    renderer_.DrawEllipseFrame(
        AlignWithRegion(ellipse, stroke_width),
        brush,
        stroke_width,
        stroke);
}


void Canvas::DrawGeometry(const Geometry& geometry) {
    const auto& state = CurrentState();
    DrawGeometry(geometry, state.brush);
}


void Canvas::DrawGeometry(const Geometry& geometry, const Brush& brush) {
    renderer_.DrawGeometry(geometry, brush, Brush{});
}


void Canvas::DrawGeometryFrame(const Geometry& geometry, float stroke_width) {
    const auto& state = CurrentState();
    DrawGeometryFrame(geometry, stroke_width, state.brush, state.stroke);
}


void Canvas::DrawGeometryFrame(const Geometry& geometry, float stroke_width, const Brush& brush) {
    const auto& state = CurrentState();
    DrawGeometryFrame(geometry, stroke_width, brush, state.stroke);
}


void Canvas::DrawGeometryFrame(
    const Geometry& geometry, 
    float stroke_width, 
    const Stroke& stroke) {

    const auto& state = CurrentState();
    DrawGeometryFrame(geometry, stroke_width, state.brush, stroke);
}


void Canvas::DrawGeometryFrame(
    const Geometry& geometry,
    float stroke_width,
    const Brush& brush,
    const Stroke& stroke) {

    Geometry drew_geometry;

    //The geometry is not aligned for line, we need to do it by setting a new transform here.
    float offset = AlignmentOffsetForLine(stroke_width, 96.f);
    if (offset != 0) {
        drew_geometry = GraphicFactory::Instance().CreateTransformedGeometry(
            geometry,
            TransformMatrix::Translation(Point(offset, offset)));
    }
    else {
        drew_geometry = geometry;
    }

    renderer_.DrawGeometryFrame(drew_geometry, brush, stroke_width, stroke);
}


void Canvas::DrawTextFormat(
    const std::wstring& text, 
    const TextFormat& text_format, 
    const Rect& rect) {

    const auto& state = CurrentState();
    DrawTextFormat(text, text_format, rect, state.brush);
}


void Canvas::DrawTextFormat(
    const std::wstring& text,
    const TextFormat& text_format,
    const Rect& rect,
    const Color& color) {

    DrawTextFormat(text, text_format, rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::DrawTextFormat(
    const std::wstring& text,
    const TextFormat& text_format,
    const Rect& rect,
    const Brush& brush) {

    renderer_.DrawTextFormat(text, text_format, AlignWithRegion(rect), brush);
}


void Canvas::DrawTextLayout(const TextLayout& text_layout, const Point& position) {
    const auto& state = CurrentState();
    DrawTextLayout(text_layout, position, state.brush);
}


void Canvas::DrawTextLayout(
    const TextLayout& text_layout, 
    const Point& position, 
    const Color& color) {

    DrawTextLayout(text_layout, position, renderer_.CreateSolidColorBrush(color));
}


void Canvas::DrawTextLayout(
    const TextLayout& text_layout, 
    const Point& position,
    const Brush& brush) {

    renderer_.DrawTextLayout(text_layout, AlignWithRegion(position), brush);
}


void Canvas::DrawBitmap(
    const RenderBitmap& bitmap,
    const Rect& destination_rect, 
    const DrawImageOptions& options) {

    renderer_.DrawBitmap(
        bitmap, 
        AlignWithRegion(destination_rect),
        options.Opacity(),
        options.InterpolationMode(),
        options.SourceRect());
}


PathGeometry Canvas::CreatePathGeometry() const {

    const auto& current_region = regions_.top();

    auto geometry = GraphicFactory::Instance().CreatePathGeometry();
    auto aligned = MakeCOMPtr<internal::AlignedPathGeometry>(
        geometry.Inner(),
        current_region.rect.position,
        current_region.aligned_rect.position,
        renderer_.GetDPI());

    return PathGeometry{ aligned };
}


RectangleGeometry Canvas::CreateRectangleGeometry(const Rect& rect) const {

    Rect aligned_rect = AlignWithRegion(rect);
    return GraphicFactory::Instance().CreateRectangleGeometry(aligned_rect);
}


RoundedRectangleGeometry Canvas::CreateRoundedRectangleGeometry(
    const RoundedRect& rounded_rect) const {

    RoundedRect aligned_rounded_rect = AlignWithRegion(rounded_rect);
    return GraphicFactory::Instance().CreateRoundedRectangleGeometry(aligned_rounded_rect);
}


EllipseGeometry Canvas::CreateEllipseGeometry(const Ellipse& ellipse) const {

    Ellipse aligned_ellipse = AlignWithRegion(ellipse);
    return GraphicFactory::Instance().CreateEllipseGeometry(ellipse);
}

}