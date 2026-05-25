#include <zaf/graphic/canvas/canvas.h>
#include <dwrite.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/d2d/path_geometry.h>
#include <zaf/graphic/d2d/rectangle_geometry.h>
#include <zaf/graphic/d2d/rounded_rectangle_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/dwrite/text_format.h>
#include <zaf/internal/graphic/alignment_helper.h>

using namespace zaf::d2d;
using namespace zaf::dwrite;

namespace zaf {

Canvas::Canvas(d2d::Renderer& renderer) : renderer_(renderer) {

    PushInitialState();
}


Canvas::~Canvas() {

}


template<typename T>
T Canvas::SnapToPixelsIfNeeded(const T& object, float stroke_width) const {

    const auto& current_state = CurrentState();
    if (current_state.pixel_snap_mode == PixelSnapMode::NoSnap) {
        return object;
    }

    const auto& current_region = regions_.top();
    T result = internal::AlignInRelatedCoordinateSystem(
        object,
        stroke_width,
        renderer_.GetDPI(),
        current_region.rect.position,
        current_region.snapped_rect.position);

    result.AddOffset(current_region.SnappedOffset());
    return result;
}


void Canvas::PushInitialState() {

    internal::CanvasStateData state_data;
    state_data.pixel_snap_mode = PixelSnapMode::Snap;
    state_data.brush = renderer_.CreateSolidColorBrush(Color::White());
    states_.push(std::move(state_data));
}


CanvasRegionGuard Canvas::PushRegion(const Rect& region_rect, const Rect& paintable_rect) {

    auto new_region = CreateNewRegion(region_rect, paintable_rect);
    regions_.push(new_region);

    // Transform should always use the non-snapped rect, in order to support non-snapped painting.
    renderer_.Transform(TransformMatrix::Translation(new_region.rect.position));

    Rect clipping_rect;
    if (CurrentState().pixel_snap_mode == PixelSnapMode::Snap) {
        clipping_rect = new_region.snapped_paintable_rect;
        clipping_rect.position -= new_region.snapped_rect.position;
        clipping_rect.position += new_region.SnappedOffset();
    }
    else {
        clipping_rect = new_region.paintable_rect;
        clipping_rect.position -= new_region.rect.position;
    }

    auto clipping_guard = InnerPushClipping(clipping_rect);
    return CanvasRegionGuard{ this, std::move(clipping_guard) };
}


internal::CanvasRegion Canvas::CreateNewRegion(
    const Rect& region_rect,
    const Rect& paintable_rect) const noexcept {

    const internal::CanvasRegion* current_region{};
    if (!regions_.empty()) {
        current_region = &regions_.top();
    }

    internal::CanvasRegion new_region;
    new_region.rect = region_rect;
    if (current_region) {
        new_region.rect.AddOffset(current_region->rect.position);
    }
    new_region.snapped_rect = SnapToPixels(new_region.rect, renderer_.GetDPI());

    new_region.paintable_rect = paintable_rect;
    if (current_region) {
        new_region.paintable_rect.AddOffset(current_region->rect.position);
        new_region.paintable_rect.Intersect(current_region->paintable_rect);
    }
    new_region.paintable_rect.Intersect(new_region.rect);
    new_region.snapped_paintable_rect = 
        SnapToPixels(new_region.paintable_rect, renderer_.GetDPI());

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
        renderer_.Transform(TransformMatrix::Translation(regions_.top().snapped_rect.position));
    }
    else {
        renderer_.Transform(TransformMatrix::Identity);
    }
}


CanvasClippingGuard Canvas::PushClipping(const Rect& clipping_rect) {
    return InnerPushClipping(SnapToPixelsIfNeeded(clipping_rect));
}


CanvasClippingGuard Canvas::InnerPushClipping(const Rect& clipping_rect) {
    renderer_.PushAxisAlignedClipping(clipping_rect, d2d::AntialiasMode::PerPrimitive);
    return CanvasClippingGuard{ this, ++current_clipping_tag_ };
}


void Canvas::PopClipping(std::size_t tag) {

    //Detect mismatch push and pop.
    ZAF_EXPECT(tag == current_clipping_tag_);

    renderer_.PopAxisAlignedClipping();
    --current_clipping_tag_;
}


CanvasState Canvas::PushState() {

    //Copy current state as new state.
    auto new_state = states_.top();
    states_.push(new_state);
    return CanvasState{ this, &states_.top(), ++state_tag_seed_ };
}


void Canvas::PopState(std::size_t tag) {

    //Detect mismatch push and pop.
    ZAF_EXPECT(tag == state_tag_seed_);

    states_.pop();
    --state_tag_seed_;
}


const internal::CanvasStateData& Canvas::CurrentState() const {
    return states_.top();
}


void Canvas::Clear() {
    renderer_.Clear();
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point) {
    const auto& state = CurrentState();
    InnerDrawLine(from_point, to_point, state.brush, state.stroke_width, state.stroke_style);
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point, const Color& color) {

    const auto& state = CurrentState();
    InnerDrawLine(
        from_point,
        to_point, 
        renderer_.CreateSolidColorBrush(color), 
        state.stroke_width,
        state.stroke_style);
}


void Canvas::DrawLine(
    const Point& from_point,
    const Point& to_point,
    const Color& color,
    float stroke_width) {

    InnerDrawLine(
        from_point,
        to_point, 
        renderer_.CreateSolidColorBrush(color), 
        stroke_width,
        CurrentState().stroke_style);
}


void Canvas::InnerDrawLine(
    const Point& from_point,
    const Point& to_point,
    const d2d::Brush& brush,
    float stroke_width,
    const d2d::StrokeStyle& stroke_style) {

    renderer_.DrawLine(
        SnapToPixelsIfNeeded(from_point, stroke_width),
        SnapToPixelsIfNeeded(to_point, stroke_width),
        brush,
        stroke_width,
        stroke_style);
}


void Canvas::FillRectangle(const Rect& rect) {
    InnerFillRectangle(rect, CurrentState().brush);
}


void Canvas::FillRectangle(const Rect& rect, const Color& color) {
    InnerFillRectangle(rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillRectangle(const Rect& rect, const d2d::Brush& brush) {
    renderer_.FillRectangle(SnapToPixelsIfNeeded(rect), brush);
}


void Canvas::DrawRectangle(const Rect& rect) {
    const auto& state = CurrentState();
    InnerDrawRectangle(rect, state.brush, state.stroke_width, state.stroke_style);
}


void Canvas::DrawRectangle(const Rect& rect, const Color& color) {
    const auto& state = CurrentState();
    InnerDrawRectangle(
        rect,
        renderer_.CreateSolidColorBrush(color),
        state.stroke_width,
        state.stroke_style);
}


void Canvas::DrawRectangle(const Rect& rect, const Color& color, float stroke_width) {
    InnerDrawRectangle(
        rect,
        renderer_.CreateSolidColorBrush(color),
        stroke_width,
        CurrentState().stroke_style);
}


void Canvas::InnerDrawRectangle(
    const Rect& rect,
    const d2d::Brush& brush,
    float stroke_width,
    const d2d::StrokeStyle& stroke_style) {

    renderer_.DrawRectangle(SnapToPixelsIfNeeded(rect, stroke_width), brush, stroke_width, stroke_style);
}


void Canvas::FillRoundedRectangle(const RoundedRect& rounded_rect) {
    InnerFillRoundedRectangle(rounded_rect, CurrentState().brush);
}


void Canvas::FillRoundedRectangle(const RoundedRect& rounded_rect, const Color& color) {
    InnerFillRoundedRectangle(rounded_rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillRoundedRectangle(const RoundedRect& rounded_rect, const d2d::Brush& brush) {
    renderer_.FillRoundedRectangle(SnapToPixelsIfNeeded(rounded_rect), brush);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect) {
    const auto& state = CurrentState();
    InnerDrawRoundedRectangle(rounded_rect, state.brush, state.stroke_width, state.stroke_style);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect, const Color& color) {
    const auto& state = CurrentState();
    InnerDrawRoundedRectangle(
        rounded_rect,
        renderer_.CreateSolidColorBrush(color),
        state.stroke_width,
        state.stroke_style);
}


void Canvas::DrawRoundedRectangle(
    const RoundedRect& rounded_rect,
    const Color& color,
    float stroke_width) {

    InnerDrawRoundedRectangle(
        rounded_rect,
        renderer_.CreateSolidColorBrush(color),
        stroke_width,
        CurrentState().stroke_style);
}


void Canvas::InnerDrawRoundedRectangle(
    const RoundedRect& rounded_rect,
    const d2d::Brush& brush,
    float stroke_width,
    const d2d::StrokeStyle& stroke_style) {

    renderer_.DrawRoundedRectangle(
        SnapToPixelsIfNeeded(rounded_rect, stroke_width),
        brush,
        stroke_width,
        stroke_style);
}


void Canvas::FillEllipse(const Ellipse& ellipse) {
    InnerFillEllipse(ellipse, CurrentState().brush);
}


void Canvas::FillEllipse(const Ellipse& ellipse, const Color& color) {
    InnerFillEllipse(ellipse, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillEllipse(const Ellipse& ellipse, const d2d::Brush& brush) {
    renderer_.FillEllipse(SnapToPixelsIfNeeded(ellipse), brush);
}


void Canvas::DrawEllipse(const Ellipse& ellipse) {
    const auto& state = CurrentState();
    InnerDrawEllipse(ellipse, state.brush, state.stroke_width, state.stroke_style);
}


void Canvas::DrawEllipse(const Ellipse& ellipse, const Color& color) {
    const auto& state = CurrentState();
    InnerDrawEllipse(
        ellipse,
        renderer_.CreateSolidColorBrush(color),
        state.stroke_width,
        state.stroke_style);
}


void Canvas::DrawEllipse(const Ellipse& ellipse, const Color& color, float stroke_width) {
    InnerDrawEllipse(
        ellipse,
        renderer_.CreateSolidColorBrush(color),
        stroke_width,
        CurrentState().stroke_style);
}


void Canvas::InnerDrawEllipse(
    const Ellipse& ellipse,
    const d2d::Brush& brush,
    float stroke_width,
    const d2d::StrokeStyle& stroke_style) {

    renderer_.DrawEllipse(
        SnapToPixelsIfNeeded(ellipse, stroke_width),
        brush,
        stroke_width,
        stroke_style);
}


void Canvas::FillGeometry(const Geometry& geometry) {
    InnerFillGeometry(geometry, CurrentState().brush);
}


void Canvas::FillGeometry(const Geometry& geometry, const Color& color) {
    InnerFillGeometry(geometry, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillGeometry(const Geometry& geometry, const d2d::Brush& brush) {
    renderer_.FillGeometry(geometry, brush, Brush{});
}


void Canvas::DrawGeometry(const Geometry& geometry) {
    const auto& state = CurrentState();
    InnerDrawGeometry(geometry, state.brush, state.stroke_width, state.stroke_style);
}


void Canvas::DrawGeometry(const Geometry& geometry, const Color& color) {

    const auto& state = CurrentState();
    InnerDrawGeometry(
        geometry,
        renderer_.CreateSolidColorBrush(color),
        state.stroke_width,
        state.stroke_style);
}


void Canvas::DrawGeometry(
    const Geometry& geometry,
    const Color& color,
    float stroke_width) {

    InnerDrawGeometry(
        geometry,
        renderer_.CreateSolidColorBrush(color),
        stroke_width,
        CurrentState().stroke_style);
}


void Canvas::InnerDrawGeometry(
    const Geometry& geometry,
    const d2d::Brush& brush,
    float stroke_width,
    const d2d::StrokeStyle& stroke_style) {

    Geometry drew_geometry;

    //The geometry is not aligned for line, we need to do it by setting a new transform here.
    float offset = SnappingPixelOffsetForLine(stroke_width, 96.f);
    if (offset != 0) {
        drew_geometry = GraphicFactory::Instance().CreateTransformedGeometry(
            geometry,
            TransformMatrix::Translation(Point(offset, offset)));
    }
    else {
        drew_geometry = geometry;
    }

    renderer_.DrawGeometry(drew_geometry, brush, stroke_width, stroke_style);
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

    renderer_.DrawTextFormat(text, text_format, SnapToPixelsIfNeeded(rect), brush);
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

    renderer_.DrawTextLayout(text_layout, SnapToPixelsIfNeeded(position), brush);
}


void Canvas::DrawBitmap(
    const d2d::RenderBitmap& bitmap,
    const Rect& destination_rect, 
    const DrawImageOptions& options) {

    renderer_.DrawBitmap(
        bitmap, 
        SnapToPixelsIfNeeded(destination_rect),
        options.Opacity(),
        options.InterpolationMode(),
        options.SourceRect());
}


PathGeometry Canvas::CreatePathGeometry() const {

    COMPtr<ID2D1PathGeometry> inner;
    auto factory = GraphicFactory::Instance().GetDirect2dFactoryHandle();
    HRESULT hresult = factory->CreatePathGeometry(inner.Reset());
    ZAF_THROW_IF_COM_ERROR(hresult);

    internal::AlignmentInfo alignment_info;
    const auto& current_region = regions_.top();
    alignment_info.coordinate_origin = current_region.rect.position;
    alignment_info.aligned_coordinate_origin = current_region.snapped_rect.position;
    alignment_info.dpi = renderer_.GetDPI();

    return PathGeometry{ std::move(inner), alignment_info };
}


RectangleGeometry Canvas::CreateRectangleGeometry(const Rect& rect) const {

    Rect aligned_rect = SnapToPixelsIfNeeded(rect);
    return GraphicFactory::Instance().CreateRectangleGeometry(aligned_rect);
}


RoundedRectangleGeometry Canvas::CreateRoundedRectangleGeometry(
    const RoundedRect& rounded_rect) const {

    RoundedRect aligned_rounded_rect = SnapToPixelsIfNeeded(rounded_rect);
    return GraphicFactory::Instance().CreateRoundedRectangleGeometry(aligned_rounded_rect);
}


EllipseGeometry Canvas::CreateEllipseGeometry(const Ellipse& ellipse) const {

    Ellipse aligned_ellipse = SnapToPixelsIfNeeded(ellipse);
    return GraphicFactory::Instance().CreateEllipseGeometry(ellipse);
}

}