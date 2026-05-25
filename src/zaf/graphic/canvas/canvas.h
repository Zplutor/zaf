#pragma once

#include <memory>
#include <stack>
#include <string>
#include <zaf/graphic/d2d/solid_color_brush.h>
#include <zaf/graphic/canvas/canvas_clipping_guard.h>
#include <zaf/graphic/canvas/canvas_region_guard.h>
#include <zaf/graphic/canvas/canvas_state.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/canvas/pixel_snap_mode.h>
#include <zaf/internal/graphic/canvas_region.h>
#include <zaf/graphic/internal/canvas_state_data.h>
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

    d2d::InterpolationMode InterpolationMode() const {
        return interpolation_mode_;
    }

    DrawImageOptions& InterpolationMode(d2d::InterpolationMode value) {
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
    d2d::InterpolationMode interpolation_mode_ = d2d::InterpolationMode::Linear;
    bool has_source_rect = false;
    Rect source_rect;
};


class Canvas : NonCopyableNonMovable {
public:
    /**
    Constructs a canvas with the specified renderer.

    @param renderer
        The renderer to which the canvas paints.

    @details
        The constructed canvas has an initial state with the following options and graphic 
        resources:
        - Pixel snap mode: Snap.
        - Brush: a solid color brush with white color.
        - Stroke width: 1.
        - StrokeStyle: default stroke style.
    */
    explicit Canvas(d2d::Renderer& renderer);
    ~Canvas();

    d2d::Renderer& Renderer() noexcept {
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
    CanvasState PushState();

    void Clear();

    void DrawLine(const Point& from_point, const Point& to_point);
    void DrawLine(const Point& from_point, const Point& to_point, const Color& color);
    void DrawLine(
        const Point& from_point,
        const Point& to_point,
        const Color& color,
        float stroke_width);

    void FillRectangle(const Rect& rect);
    void FillRectangle(const Rect& rect, const Color& color);

    void DrawRectangle(const Rect& rect);
    void DrawRectangle(const Rect& rect, const Color& color);
    void DrawRectangle(const Rect& rect, const Color& color, float stroke_width);

    void FillRoundedRectangle(const RoundedRect& rounded_rect);
    void FillRoundedRectangle(const RoundedRect& rounded_rect, const Color& color);
    
    void DrawRoundedRectangle(const RoundedRect& rounded_rect);
    void DrawRoundedRectangle(const RoundedRect& rounded_rect, const Color& color);
    void DrawRoundedRectangle(
        const RoundedRect& rounded_rect,
        const Color& color,
        float stroke_width);

    void FillEllipse(const Ellipse& ellipse);
    void FillEllipse(const Ellipse& ellipse, const Color& color);
    
    void DrawEllipse(const Ellipse& ellipse);
    void DrawEllipse(const Ellipse& ellipse, const Color& color);
    void DrawEllipse(const Ellipse& ellipse, const Color& color, float stroke_width);

    void FillGeometry(const d2d::Geometry& geometry);
    void FillGeometry(const d2d::Geometry& geometry, const Color& color);

    void DrawGeometry(const d2d::Geometry& geometry);
    void DrawGeometry(const d2d::Geometry& geometry, const Color& color);
    void DrawGeometry(
        const d2d::Geometry& geometry,
        const Color& color,
        float stroke_width);

    void DrawTextFormat(
        const std::wstring& text,
        const dwrite::TextFormat& text_format, 
        const Rect& rect);
    void DrawTextFormat(
        const std::wstring& text,
        const dwrite::TextFormat& text_format,
        const Rect& rect,
        const Color& color);
    void DrawTextFormat(
        const std::wstring& text, 
        const dwrite::TextFormat& text_format,
        const Rect& rect,
        const d2d::Brush& brush);

    void DrawTextLayout(const dwrite::TextLayout& text_layout, const Point& position);
    void DrawTextLayout(
        const dwrite::TextLayout& text_layout,
        const Point& position, 
        const Color& color);
    void DrawTextLayout(
        const dwrite::TextLayout& text_layout,
        const Point& position, 
        const d2d::Brush& brush);

    void DrawBitmap(
        const d2d::RenderBitmap& bitmap,
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
    friend class CanvasState;

    void PopRegion(CanvasClippingGuard&& clipping_guard);
    void PopClipping(std::size_t tag);
    void PopState(std::size_t tag);

private:
    void PushInitialState();

    internal::CanvasRegion CreateNewRegion(
        const Rect& region_rect,
        const Rect& paintable_rect) const noexcept;

    CanvasClippingGuard InnerPushClipping(const Rect& clipping_rect);

    const internal::CanvasStateData& CurrentState() const;

    template<typename T>
    T SnapToPixelsIfNeeded(const T& object, float stroke_width = 0) const;

    void InnerDrawLine(
        const Point& from_point,
        const Point& to_point,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style);

    void InnerFillRectangle(const Rect& rect, const d2d::Brush& brush);
    void InnerDrawRectangle(
        const Rect& rect,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style);

    void InnerFillRoundedRectangle(const RoundedRect& rounded_rect, const d2d::Brush& brush);
    void InnerDrawRoundedRectangle(
        const RoundedRect& rounded_rect,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style);

    void InnerFillEllipse(const Ellipse& ellipse, const d2d::Brush& brush);
    void InnerDrawEllipse(
        const Ellipse& ellipse,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style);

    void InnerFillGeometry(const d2d::Geometry& geometry, const d2d::Brush& brush);
    void InnerDrawGeometry(
        const d2d::Geometry& geometry,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style);

private:
    d2d::Renderer renderer_;
    std::stack<internal::CanvasRegion> regions_;
    std::size_t current_clipping_tag_{};

    std::stack<internal::CanvasStateData> states_;
    std::size_t state_tag_seed_{};
};

}