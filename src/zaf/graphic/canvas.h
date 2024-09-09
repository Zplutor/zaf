#pragma once

#include <memory>
#include <stack>
#include <string>
#include <zaf/graphic/d2d/solid_color_brush.h>
#include <zaf/graphic/canvas_clipping_guard.h>
#include <zaf/graphic/canvas_region_guard.h>
#include <zaf/graphic/canvas_state_guard.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/internal/alignment_helper.h>
#include <zaf/graphic/internal/canvas_region.h>
#include <zaf/graphic/internal/canvas_state.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/d2d/renderer.h>

namespace zaf {
namespace d2d {
class EllipseGeometry;
class PathGeometry;
class RectangleGeometry;
class RoundedRectangleGeometry;
}

class DrawImageOptions {
public:
    float Opacity() const {
        return opacity_;
    }

    DrawImageOptions& Opacity(float value) {
        opacity_ = value;
        return *this;
    }

    InterpolationMode InterpolationMode() const {
        return interpolation_mode_;
    }

    DrawImageOptions& InterpolationMode(zaf::InterpolationMode value) {
        interpolation_mode_ = value;
        return *this;
    }

    const Rect* SourceRect() const {
        return has_source_rect ? &source_rect : nullptr;
    }

    DrawImageOptions& SourceRect(const Rect& value) {
        has_source_rect = true;
        source_rect = value;
        return *this;
    }

private:
    float opacity_ = 1.f;
    zaf::InterpolationMode interpolation_mode_ = InterpolationMode::Linear;
    bool has_source_rect = false;
    Rect source_rect;
};


class Canvas : NonCopyableNonMovable {
public:
    /**
    Constructs a canvas with specified renderer and rects.

    @param renderer
        The renderer to which the canvas paints to.
    */
    explicit Canvas(d2d::Renderer& renderer);
    ~Canvas();

    d2d::Renderer& Renderer() {
        return renderer_;
    }

    /**
    Pushes a new region into canvas.

    @param region_rect
        The rectangle area of the new region, in current region coordinate. After pushing, the 
        orgin of coordinate of the canvas moves to the position of this rectangle area.

    @param paintable_rect
        The rectangle area that can be painted in the new region, in current region coordinate. 
        It's normally a sub area of region_rect.

    @return 
        A guard that auto pops the region on destruction. You should keep the guard during painting
        in the region and destroy it after painting.
    */
    [[nodiscard]]
    CanvasRegionGuard PushRegion(const Rect& region_rect, const Rect& paintable_rect);

    /**
    Pushes a clipping area into canvas. 

    @param clipping_rect
        The retangle area of the clipping, in current region coordinate. After pushing, only 
        painting operations that occur in this area have effect.

    @return 
        A guard that auto pops the clipping on destruction. You should keep the guard during 
        painting with the clipping and destroy it after painting.
    */
    [[nodiscard]]
    CanvasClippingGuard PushClipping(const Rect& clipping_rect);

    [[nodiscard]]
    CanvasStateGuard PushState();

    void SetBrush(const d2d::Brush& brush);
    void SetBrushWithColor(const Color& color);
    void SetStroke(const d2d::Stroke& stroke);

    void Clear();

    void DrawLine(const Point& from_point, const Point& to_point, float stroke_width);

    void DrawRectangle(const Rect& rect);
    void DrawRectangle(const Rect& rect, const Color& color);
    void DrawRectangle(const Rect& rect, const d2d::Brush& brush);

    void DrawRectangleFrame(const Rect& rect, float stroke_width);
    void DrawRectangleFrame(const Rect& rect, float stroke_width, const d2d::Brush& brush);
    void DrawRectangleFrame(const Rect& rect, float stroke_width, const d2d::Stroke& stroke);
    void DrawRectangleFrame(
        const Rect& rect,
        float stroke_width,
        const d2d::Brush& brush,
        const d2d::Stroke& stroke);

    void DrawRoundedRectangle(const RoundedRect& rounded_rect);
    void DrawRoundedRectangle(const RoundedRect& rounded_rect, const Color& color);
    void DrawRoundedRectangle(const RoundedRect& rounded_rect, const d2d::Brush& brush);

    void DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width);
    void DrawRoundedRectangleFrame(
        const RoundedRect& rounded_rect, 
        float stroke_width, 
        const d2d::Brush& brush);
    void DrawRoundedRectangleFrame(
        const RoundedRect& rounded_rect,
        float stroke_width,
        const d2d::Stroke& stroke);
    void DrawRoundedRectangleFrame(
        const RoundedRect& rounded_rect,
        float stroke_width,
        const d2d::Brush& brush,
        const d2d::Stroke& stroke);

    void DrawEllipse(const Ellipse& ellipse);
    void DrawEllipse(const Ellipse& ellipse, const d2d::Brush& brush);

    void DrawEllipseFrame(const Ellipse& ellipse, float stroke_width);
    void DrawEllipseFrame(const Ellipse& ellipse, float stroke_width, const d2d::Brush& brush);
    void DrawEllipseFrame(const Ellipse& ellipse, float stroke_width, const d2d::Stroke& stroke);
    void DrawEllipseFrame(
        const Ellipse& ellipse, 
        float stroke_width, 
        const d2d::Brush& brush,
        const d2d::Stroke& stroke);

    void DrawGeometry(const d2d::Geometry& geometry);
    void DrawGeometry(const d2d::Geometry& geometry, const d2d::Brush& brush);

    void DrawGeometryFrame(const d2d::Geometry& geometry, float stroke_width);
    void DrawGeometryFrame(
        const d2d::Geometry& geometry, 
        float stroke_width, 
        const d2d::Brush& brush);

    void DrawGeometryFrame(
        const d2d::Geometry& geometry,
        float stroke_width, 
        const d2d::Stroke& stroke);

    void DrawGeometryFrame(
        const d2d::Geometry& geometry,
        float stroke_width, 
        const d2d::Brush& brush,
        const d2d::Stroke& stroke);

    void DrawTextFormat(const std::wstring& text, const TextFormat& text_format, const Rect& rect);
    void DrawTextFormat(
        const std::wstring& text,
        const TextFormat& text_format,
        const Rect& rect,
        const Color& color);
    void DrawTextFormat(
        const std::wstring& text, 
        const TextFormat& text_format, 
        const Rect& rect,
        const d2d::Brush& brush);

    void DrawTextLayout(const TextLayout& text_layout, const Point& position);
    void DrawTextLayout(const TextLayout& text_layout, const Point& position, const Color& color);
    void DrawTextLayout(
        const TextLayout& text_layout, 
        const Point& position, 
        const d2d::Brush& brush);

    void DrawBitmap(
        const RenderBitmap& bitmap,
        const Rect& destination_rect, 
        const DrawImageOptions& options = {});

    d2d::PathGeometry CreatePathGeometry() const;
    d2d::RectangleGeometry CreateRectangleGeometry(const Rect& rect) const;
    d2d::RoundedRectangleGeometry CreateRoundedRectangleGeometry(
        const RoundedRect& rounded_rect) const;
    d2d::EllipseGeometry CreateEllipseGeometry(const Ellipse& ellipse) const;

private:
    friend class CanvasClippingGuard;
    friend class CanvasRegionGuard;
    friend class CanvasStateGuard;

    void PopRegion(CanvasClippingGuard&& clipping_guard);
    void PopClipping(std::size_t tag);
    void PopState(std::size_t tag);

private:
    internal::CanvasRegion CreateNewRegion(
        const Rect& region_rect,
        const Rect& paintable_rect) const;

    CanvasClippingGuard InnerPushClipping(const Rect& clipping_rect);

    const internal::CanvasState& CurrentState() const;

    template<typename T>
    T AlignWithRegion(const T& object, float stroke_width = 0) const {

        const auto& current_region = regions_.top();
        return internal::AlignInRelatedCoordinateSystem(
            object, 
            stroke_width, 
            renderer_.GetDPI(),
            current_region.rect.position, 
            current_region.aligned_rect.position);
    }

private:
    d2d::Renderer renderer_;
    std::stack<internal::CanvasRegion> regions_;
    std::size_t current_clipping_tag_{};
    std::stack<internal::CanvasState> states_;
    std::size_t current_state_tag_{};
};

}