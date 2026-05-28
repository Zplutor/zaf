#include <zaf/graphic/canvas/canvas.h>
#include <dwrite.h>
#include <zaf/base/error/com_error.h>
#include <zaf/graphic/pixel_snapping.h>
#include <zaf/graphic/d2d/path_geometry.h>
#include <zaf/graphic/d2d/rectangle_geometry.h>
#include <zaf/graphic/d2d/rounded_rectangle_geometry.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/dwrite/text_format.h>
#include <zaf/graphic/internal/snapped_path_geometry.h>
#include <zaf/graphic/internal/snapping_helper.h>

using namespace zaf::d2d;
using namespace zaf::dwrite;

namespace zaf {

Canvas::Canvas(d2d::Renderer& renderer) : renderer_(renderer) {

    PushInitialState();
}


Canvas::~Canvas() {

}


template<typename T>
T Canvas::SnapToPixelsInCurrentRegion(const T& object, float stroke_width) const noexcept {

    const auto& current_region = regions_.top();
    return internal::SnapInCoordinateSystem(
        object,
        stroke_width,
        renderer_.GetDPI(),
        current_region.rect.position,
        current_region.snapped_rect.position);
}


template<typename T>
T Canvas::SnapToPixelsIfNeeded(const T& object, float stroke_width) const noexcept {

    const auto& current_state = CurrentState();
    if (current_state.pixel_snap_mode == PixelSnapMode::NoSnap) {
        return object;
    }
    return SnapToPixelsInCurrentRegion(object, stroke_width);
}


void Canvas::PushInitialState() {

    internal::CanvasStateData state_data;
    state_data.pixel_snap_mode = PixelSnapMode::Snap;
    state_data.brush = renderer_.CreateSolidColorBrush(Color::White());
    states_.push(std::move(state_data));
}


CanvasRegion Canvas::PushRegion(const Rect& region_rect, const Rect& paintable_rect) {

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
    return CanvasRegion{ this, std::move(clipping_guard) };
}


internal::CanvasRegionData Canvas::CreateNewRegion(
    const Rect& region_rect,
    const Rect& paintable_rect) const noexcept {

    const internal::CanvasRegionData* current_region{};
    if (!regions_.empty()) {
        current_region = &regions_.top();
    }

    internal::CanvasRegionData new_region;
    new_region.rect = region_rect;
    if (current_region) {
        new_region.rect.AddOffset(current_region->rect.position);
    }
    new_region.snapped_rect = zaf::SnapToPixels(new_region.rect, renderer_.GetDPI());

    new_region.paintable_rect = paintable_rect;
    if (current_region) {
        new_region.paintable_rect.AddOffset(current_region->rect.position);
        new_region.paintable_rect.Intersect(current_region->paintable_rect);
    }
    new_region.paintable_rect.Intersect(new_region.rect);
    new_region.snapped_paintable_rect = 
        zaf::SnapToPixels(new_region.paintable_rect, renderer_.GetDPI());

    return new_region;
}


void Canvas::PopRegion(CanvasClipping&& clipping_guard) {

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


CanvasClipping Canvas::PushClipping(const Rect& clipping_rect) noexcept {
    return InnerPushClipping(SnapToPixelsIfNeeded(clipping_rect));
}


CanvasClipping Canvas::InnerPushClipping(const Rect& clipping_rect) noexcept {
    renderer_.PushAxisAlignedClipping(clipping_rect, d2d::AntialiasMode::PerPrimitive);
    return CanvasClipping{ this, ++current_clipping_tag_ };
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
    return CanvasState{ this, &states_.top(), ++current_state_tag_ };
}


void Canvas::PopState(std::size_t tag) {

    //Detect mismatch push and pop.
    ZAF_EXPECT(tag == current_state_tag_);

    states_.pop();
    --current_state_tag_;
}


const internal::CanvasStateData& Canvas::CurrentState() const noexcept {
    return states_.top();
}


void Canvas::Clear() noexcept {
    renderer_.Clear();
}


void Canvas::DrawLine(const Point& from_point, const Point& to_point) noexcept {
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
    const d2d::StrokeStyle& stroke_style) noexcept {

    renderer_.DrawLine(
        SnapToPixelsIfNeeded(from_point, stroke_width),
        SnapToPixelsIfNeeded(to_point, stroke_width),
        brush,
        stroke_width,
        stroke_style);
}


void Canvas::FillRectangle(const Rect& rect) noexcept {
    InnerFillRectangle(rect, CurrentState().brush);
}


void Canvas::FillRectangle(const Rect& rect, const Color& color) {
    InnerFillRectangle(rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillRectangle(const Rect& rect, const d2d::Brush& brush) noexcept {
    renderer_.FillRectangle(SnapToPixelsIfNeeded(rect), brush);
}


void Canvas::DrawRectangle(const Rect& rect) noexcept {
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
    const d2d::StrokeStyle& stroke_style) noexcept {

    renderer_.DrawRectangle(SnapToPixelsIfNeeded(rect, stroke_width), brush, stroke_width, stroke_style);
}


void Canvas::FillRoundedRectangle(const RoundedRect& rounded_rect) noexcept {
    InnerFillRoundedRectangle(rounded_rect, CurrentState().brush);
}


void Canvas::FillRoundedRectangle(const RoundedRect& rounded_rect, const Color& color) {
    InnerFillRoundedRectangle(rounded_rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillRoundedRectangle(
    const RoundedRect& rounded_rect,
    const d2d::Brush& brush) noexcept {

    renderer_.FillRoundedRectangle(SnapToPixelsIfNeeded(rounded_rect), brush);
}


void Canvas::DrawRoundedRectangle(const RoundedRect& rounded_rect) noexcept {
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
    const d2d::StrokeStyle& stroke_style) noexcept {

    renderer_.DrawRoundedRectangle(
        SnapToPixelsIfNeeded(rounded_rect, stroke_width),
        brush,
        stroke_width,
        stroke_style);
}


void Canvas::FillEllipse(const Ellipse& ellipse) noexcept {
    InnerFillEllipse(ellipse, CurrentState().brush);
}


void Canvas::FillEllipse(const Ellipse& ellipse, const Color& color) {
    InnerFillEllipse(ellipse, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillEllipse(const Ellipse& ellipse, const d2d::Brush& brush) noexcept {
    renderer_.FillEllipse(SnapToPixelsIfNeeded(ellipse), brush);
}


void Canvas::DrawEllipse(const Ellipse& ellipse) noexcept {
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
    const d2d::StrokeStyle& stroke_style) noexcept {

    renderer_.DrawEllipse(
        SnapToPixelsIfNeeded(ellipse, stroke_width),
        brush,
        stroke_width,
        stroke_style);
}


void Canvas::FillGeometry(const Geometry& geometry) {
    ZAF_EXPECT(geometry);
    InnerFillGeometry(geometry, CurrentState().brush);
}


void Canvas::FillGeometry(const Geometry& geometry, const Color& color) {
    ZAF_EXPECT(geometry);
    InnerFillGeometry(geometry, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerFillGeometry(const Geometry& geometry, const d2d::Brush& brush) noexcept {
    renderer_.FillGeometry(ExtractSnappedGeometry(geometry), brush, Brush{});
}


void Canvas::DrawGeometry(const Geometry& geometry) {
    ZAF_EXPECT(geometry);
    const auto& state = CurrentState();
    InnerDrawGeometry(geometry, state.brush, state.stroke_width, state.stroke_style);
}


void Canvas::DrawGeometry(const Geometry& geometry, const Color& color) {
    ZAF_EXPECT(geometry);
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

    ZAF_EXPECT(geometry);
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
    const d2d::StrokeStyle& stroke_style) noexcept {

    renderer_.DrawGeometry(ExtractSnappedGeometry(geometry), brush, stroke_width, stroke_style);
}


d2d::Geometry Canvas::ExtractSnappedGeometry(const d2d::Geometry& geometry) const noexcept {

    // If the geometry is created by `CreateSnappedPathGeometry`, it is a wrapper object that
    // isn't created by the same factory as renderer. But Direct2D requires that when drawing
    // geometry, the geometry must be created by the same factory as renderer. 
    // So we need to extract the inner geometry from the wrapper object.

    auto snapped_path_geometry = 
        geometry.Ptr().Query<internal::SnappedPathGeometry>(internal::IID_SnappedPathGeometry);

    if (snapped_path_geometry) {
        return d2d::Geometry{ snapped_path_geometry->Inner() };
    }

    return geometry;
}


void Canvas::DrawTextFormat(
    const std::wstring& text, 
    const TextFormat& text_format, 
    const Rect& rect) {

    ZAF_EXPECT(text_format);

    const auto& state = CurrentState();
    InnerDrawTextFormat(text, text_format, rect, state.brush);
}


void Canvas::DrawTextFormat(
    const std::wstring& text,
    const TextFormat& text_format,
    const Rect& rect,
    const Color& color) {

    ZAF_EXPECT(text_format);

    InnerDrawTextFormat(text, text_format, rect, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerDrawTextFormat(
    const std::wstring& text,
    const TextFormat& text_format,
    const Rect& rect,
    const Brush& brush) noexcept {

    renderer_.DrawTextFormat(text, text_format, SnapToPixelsIfNeeded(rect), brush);
}


void Canvas::DrawTextLayout(const TextLayout& text_layout, const Point& position) {
    ZAF_EXPECT(text_layout);
    InnerDrawTextLayout(text_layout, position, CurrentState().brush);
}


void Canvas::DrawTextLayout(
    const TextLayout& text_layout, 
    const Point& position, 
    const Color& color) {

    ZAF_EXPECT(text_layout);
    InnerDrawTextLayout(text_layout, position, renderer_.CreateSolidColorBrush(color));
}


void Canvas::InnerDrawTextLayout(
    const TextLayout& text_layout, 
    const Point& position,
    const Brush& brush) noexcept {

    renderer_.DrawTextLayout(text_layout, SnapToPixelsIfNeeded(position), brush);
}


void Canvas::DrawBitmap(
    const d2d::RenderBitmap& bitmap,
    const Rect& destination_rect, 
    const DrawImageOptions& options) {

    ZAF_EXPECT(bitmap);

    renderer_.DrawBitmap(
        bitmap, 
        SnapToPixelsIfNeeded(destination_rect),
        options.Opacity(),
        options.InterpolationMode(),
        options.SourceRect());
}


Point Canvas::SnapToPixels(const Point& point, float stroke_width) const noexcept {
    return SnapToPixelsInCurrentRegion(point, stroke_width);
}


Rect Canvas::SnapToPixels(const Rect& rect, float stroke_width) const noexcept {
    return SnapToPixelsInCurrentRegion(rect, stroke_width);
}


RoundedRect Canvas::SnapToPixels(
    const RoundedRect& rounded_rect, 
    float stroke_width) const noexcept {

    return SnapToPixelsInCurrentRegion(rounded_rect, stroke_width);
}


Ellipse Canvas::SnapToPixels(const Ellipse& ellipse, float stroke_width) const noexcept {
    return SnapToPixelsInCurrentRegion(ellipse, stroke_width);
}


d2d::RectangleGeometry Canvas::CreateSnappedRectangleGeometry(
    const Rect& rect,
    float stroke_width) const {

    auto snapped_rect = SnapToPixelsInCurrentRegion(rect, stroke_width);
    return GraphicFactory::Instance().CreateRectangleGeometry(snapped_rect);
}


RoundedRectangleGeometry Canvas::CreateSnappedRoundedRectangleGeometry(
    const RoundedRect& rounded_rect,
    float stroke_width) const {

    auto snapped_rounded_rect = SnapToPixelsInCurrentRegion(rounded_rect, stroke_width);
    return GraphicFactory::Instance().CreateRoundedRectangleGeometry(snapped_rounded_rect);
}


EllipseGeometry Canvas::CreateSnappedEllipseGeometry(
    const Ellipse& ellipse,
    float stroke_width) const {

    Ellipse snapped_ellipse = SnapToPixelsInCurrentRegion(ellipse, stroke_width);
    return GraphicFactory::Instance().CreateEllipseGeometry(snapped_ellipse);
}


d2d::PathGeometry Canvas::CreateSnappedPathGeometry(float stroke_width) const {

    auto path_geometry = GraphicFactory::Instance().CreatePathGeometry();
    auto inner = path_geometry.Inner();

    internal::GeometrySnapData snap_data;
    snap_data.stroke_width = stroke_width;
    snap_data.dpi = renderer_.GetDPI();
    if (!regions_.empty()) {
        const auto& current_region = regions_.top();
        snap_data.coordinate_origin = current_region.rect.position;
        snap_data.snapped_coordinate_origin = current_region.snapped_rect.position;
    }

    COMPtr<ID2D1PathGeometry> snapped_geometry{ 
        new internal::SnappedPathGeometry(std::move(inner), snap_data)
    };
    return PathGeometry{ snapped_geometry };
}

}