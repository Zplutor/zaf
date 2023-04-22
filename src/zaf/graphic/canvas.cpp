#include <zaf/graphic/canvas.h>
#include <dwrite.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/alignment.h>
#include <zaf/graphic/geometry/path_geometry.h>
#include <zaf/graphic/geometry/rectangle_geometry.h>
#include <zaf/graphic/geometry/rounded_rectangle_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/text/text_format.h>

namespace zaf {

Canvas::Canvas(zaf::Renderer& renderer) : renderer_(renderer) {

    //Add an initial state.
    states_.push(internal::CanvasState{});
}


Canvas::~Canvas() {

}


CanvasRegionGuard Canvas::PushRegion(const Rect& region_rect, const Rect& paintable_rect) {

    auto new_region = CreateNewRegion(region_rect, paintable_rect);
    regions_.push(new_region);

    renderer_.Transform(TransformMatrix::Translation(new_region.aligned_rect.position));

    Rect clipping_rect = new_region.aligned_paintable_rect;
    clipping_rect.SubtractOffset(new_region.aligned_rect.position);
    auto clipping_guard = PushClipping(clipping_rect);

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
    new_region.aligned_rect = Align(new_region.rect);

    new_region.paintable_rect = paintable_rect;
    if (current_region) {
        new_region.paintable_rect.AddOffset(current_region->rect.position);
        new_region.paintable_rect.Intersect(current_region->paintable_rect);
    }
    new_region.paintable_rect.Intersect(new_region.rect);
    new_region.aligned_paintable_rect = Align(new_region.paintable_rect);
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
    renderer_.PushAxisAlignedClipping(aligned_clipping_rect, zaf::AntialiasMode::PerPrimitive);

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


Brush Canvas::GetCurrentBrush() {

    auto& current_state = states_.top();
    if (current_state.brush) {
        return current_state.brush;
    }
}


Stroke Canvas::GetCurrentStroke() {

}


void Canvas::SaveState() {

    auto new_state = std::make_shared<State>();
    new_state->brush = renderer_.CreateSolidColorBrush(Color::White());

    states_.push_back(new_state);
}


void Canvas::RestoreState() {

    if (! states_.empty()) {

        auto current_state = states_.back();
        states_.pop_back();
    }
    else {
        ZAF_ALERT();
    }
}


std::shared_ptr<Canvas::State> Canvas::GetCurrentState() const {

    if (! states_.empty()) {
        return states_.back();
    }
    else {
        ZAF_ALERT();
        return nullptr;
    }
}


void Canvas::Clear() {
    renderer_.Clear();
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point, float stroke_width) {

    auto state = GetCurrentState();
    renderer_.DrawLine(
        AlignWithRegion(from_point, stroke_width),
        AlignWithRegion(to_point, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawRectangle(const Rect& rect) {
    auto state = GetCurrentState();
    renderer_.DrawRectangle(AlignWithRegion(rect), state->brush);
}


void Canvas::DrawRectangleFrame(const Rect& rect, float stroke_width) {
    auto state = GetCurrentState();
    renderer_.DrawRectangleFrame(
        AlignWithRegion(rect, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect) {
    auto state = GetCurrentState();
    renderer_.DrawRoundedRectangle(AlignWithRegion(rounded_rect), state->brush);
}


void Canvas::DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width) {

    auto state = GetCurrentState();
    renderer_.DrawRoundedRectangleFrame(
        AlignWithRegion(rounded_rect, stroke_width),
        state->brush,
        stroke_width,
        state->stroke);
}


void Canvas::DrawEllipse(const Ellipse& ellipse) {
    auto state = GetCurrentState();
    renderer_.DrawEllipse(AlignWithRegion(ellipse), state->brush);
}


void Canvas::DrawEllipseFrame(const Ellipse& ellipse, float stroke_width) {
    auto state = GetCurrentState();
    renderer_.DrawEllipseFrame(
        AlignWithRegion(ellipse, stroke_width),
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
        drew_geometry = GraphicFactory::Instance().CreateTransformedGeometry(
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

    renderer_.DrawTextFormat(
        text, 
        text_format, 
        AlignWithRegion(rect),
        GetCurrentState()->brush);
}


void Canvas::DrawTextLayout(const TextLayout& text_layout, const Point& position) {

    renderer_.DrawTextLayout(
        text_layout, 
        AlignWithRegion(position), 
        GetCurrentState()->brush);
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

    ID2D1PathGeometry* handle{};
    HRESULT result = 
        GraphicFactory::Instance().GetDirect2dFactoryHandle()->CreatePathGeometry(&handle);

    ZAF_THROW_IF_COM_ERROR(result);

    const auto& current_region = regions_.top();
    return PathGeometry(
        handle, 
        current_region.rect.position, 
        current_region.aligned_rect.position);
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