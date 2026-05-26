#pragma once

/**
@file
    Defines the `zaf::Canvas` class.
*/

#include <stack>
#include <string>
#include <zaf/graphic/canvas/canvas_clipping.h>
#include <zaf/graphic/canvas/canvas_region.h>
#include <zaf/graphic/canvas/canvas_state.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/canvas/pixel_snap_mode.h>
#include <zaf/graphic/d2d/brush.h>
#include <zaf/graphic/internal/canvas_region_data.h>
#include <zaf/graphic/internal/canvas_state_data.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/d2d/renderer.h>

namespace zaf::d2d {
class EllipseGeometry;
class PathGeometry;
class RectangleGeometry;
class RoundedRectangleGeometry;
}

namespace zaf {

class DrawImageOptions {
public:
    float Opacity() const noexcept {
        return opacity_;
    }

    DrawImageOptions& Opacity(float value) noexcept {
        opacity_ = value;
        return *this;
    }

    d2d::InterpolationMode InterpolationMode() const noexcept {
        return interpolation_mode_;
    }

    DrawImageOptions& InterpolationMode(d2d::InterpolationMode value) noexcept {
        interpolation_mode_ = value;
        return *this;
    }

    const Rect* SourceRect() const noexcept {
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


/**
Provides rich functionalities for drawing graphics in a renderer.

@details
    <b>About State</b>

    A canvas maintains a stack of states. Each state contains the options that affect the painting
    operations, such as pixel snap mode, and the graphic resources that used for painting, such as 
    brush and stroke style.

    To change the state of canvas, users must call `PushState()` first to create a new state, and
    call its setter methods. Subsequent painting operations will use the new state. Some painting
    methods have overloads that allow users to specify state parameters to override the current 
    state for the specific painting operation, without changing the current state.

    <b>About Pixel Snap Mode</b>

    Pixel snap mode is an option that affects how the canvas snaps the coordinates of the painting
    operations to pixels. When the pixel snap mode is `Snap`, the canvas tries to snap the painting
    to pixels to get sharper rendering result, but it could lead to misalignment among different 
    painting operations. When the pixel snap mode is `NoSnap`, the canvas doesn't snap the painting
    to pixels, which may cause blurry rendering result, but it can keep the alignment among 
    different painting operations, and enables sub-pixel rendering.

    The default pixel snap mode of canvas is `Snap`, as it is mainly used for painting UI elements,
    which usually requires sharper rendering result.

    <b> About Region and Clipping</b>

    A canvas maintains a stack of regions. A region is a rectangle area that transforms the 
    coordinate origin and limits the painting area. When a region is pushed into canvas, the 
    coordinate origin moves to the position of the region, and subsequent painting operations are 
    in the coordinate system of the region. A region also has a paintable area, which is a sub area
    of the region. Only painting operations that occur in the paintable area have effect.

    A canvas maintains a stack of clippings as well. A clipping is a rectangle area that limits the
    painting area but doesn't transform the coordinate origin. The paintable area of a region is 
    actually a clipping that is automatically pushed into canvas when the region is pushed.
*/
class Canvas : NonCopyableNonMovable {
public:
    /**
    Constructs a canvas with the specified renderer.

    @param renderer
        The renderer to which the canvas paints.

    @throw std::bad_alloc
    @throw zaf::COMError
        Thrown if creating graphic resources for the initial state fails.

    @details
        The constructed canvas has an initial state with the following options and graphic 
        resources:
        - Pixel snap mode: Snap.
        - Brush: a solid color brush with white color.
        - Stroke width: 1.
        - Stroke style: null, which means using the default stroke style of renderer.
    */
    explicit Canvas(d2d::Renderer& renderer);
    ~Canvas();

    /**
    Gets the renderer to which the canvas paints.
    */
    d2d::Renderer& Renderer() noexcept {
        return renderer_;
    }

    /**
    Pushes a new state into canvas.

    @return
        A `CanvasState` instance inherited from current state, with the same options and graphic 
        resources. It auto pops the state on destruction. Users should keep the instance during 
        painting with the state and destroy it after painting.

    @throw std::bad_alloc

    @details
        @warning
        The push/pop of state must be in pair, otherwise the behavior is undefined.
    */
    [[nodiscard]]
    CanvasState PushState();

    /**
    Pushes a new region into canvas.

    @param region_rect
        The rectangle area of the new region, in current region coordinate. After pushing, the 
        coordinate origin of the canvas moves to the position of this rectangle area.

    @param paintable_rect
        The rectangle area that can be painted in the new region, in current region coordinate. 
        It's usually a sub area of `region_rect`.

    @return
        A `CanvasRegion` instance that auto pops the region on destruction. Users should keep the 
        instance during painting in the region and destroy it after painting.

    @throw std::bad_alloc

    @details
        This method pushes a new clipping for the paintable area of the new region internally. The 
        clipping rectangle will be snapped to pixels if current state's pixel snap mode is set to
        `Snap`.

        @warning
        The push/pop of region must be in pair, otherwise the behavior is undefined.
    */
    [[nodiscard]]
    CanvasRegion PushRegion(const Rect& region_rect, const Rect& paintable_rect);

    /**
    Pushes a new clipping area into canvas. 

    @param clipping_rect
        The retangle area of the clipping, in current region coordinate. After pushing, only 
        painting operations that occur in this area have effect.

    @return 
        A `CanvasClipping` instance that auto pops the clipping on destruction. Users should keep
        the instance during painting with the clipping and destroy it after painting.

        @warning
        The push/pop of clipping must be in pair, otherwise the behavior is undefined.
    */
    [[nodiscard]]
    CanvasClipping PushClipping(const Rect& clipping_rect) noexcept;

    /**
    Clears the painting area of canvas with transparent color.

    @details
        If there is a clipping in canvas, only the area in the clipping will be cleared.
    */
    void Clear() noexcept;

    /**
    Draws a line between two points, using the brush, stroke width and stroke style in current
    state.

    @param from_point
        The starting point of the line, in current region coordinate.

    @param to_point
        The ending point of the line, in current region coordinate.
    */
    void DrawLine(const Point& from_point, const Point& to_point) noexcept;

    /**
    Draws a line between two points with the specified color, using the stroke width and stroke 
    style in current state.

    @param from_point
        The starting point of the line, in current region coordinate.

    @param to_point
        The ending point of the line, in current region coordinate.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawLine(const Point& from_point, const Point& to_point, const Color& color);

    /**
    Draws a line between two points with the specified color and stroke width, using the stroke
    style in current state.

    @param from_point
        The starting point of the line, in current region coordinate.

    @param to_point
        The ending point of the line, in current region coordinate.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawLine(
        const Point& from_point,
        const Point& to_point,
        const Color& color,
        float stroke_width);

    /**
    Fills a rectangle, using the brush in current state.

    @param rect
        The rectangle to fill, in current region coordinate.
    */
    void FillRectangle(const Rect& rect) noexcept;

    /**
    Fills a rectangle with the specified color.

    @param rect
        The rectangle to fill, in current region coordinate.

    @param color
        The color used to fill the rectangle.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void FillRectangle(const Rect& rect, const Color& color);

    /**
    Draws the outline of a rectangle, using the brush, stroke width and stroke style in current
    state.

    @param rect
        The rectangle to draw, in current region coordinate.
    */
    void DrawRectangle(const Rect& rect) noexcept;

    /**
    Draws the outline of a rectangle with the specified color, using the stroke width and stroke
    style in current state.

    @param rect
        The rectangle to draw, in current region coordinate.

    @param color
        The color used to draw the rectangle.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawRectangle(const Rect& rect, const Color& color);

    /**
    Draws the outline of a rectangle with the specified color and stroke width, using the stroke
    style in current state.

    @param rect
        The rectangle to draw, in current region coordinate.

    @param color
        The color used to draw the rectangle.

    @param stroke_width
        The width of stroke.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawRectangle(const Rect& rect, const Color& color, float stroke_width);

    /**
    Fills a rounded rectangle, using the brush in current state.

    @param rounded_rect
        The rounded rectangle to fill, in current region coordinate.
    */
    void FillRoundedRectangle(const RoundedRect& rounded_rect) noexcept;

    /**
    Fills a rounded rectangle with the specified color.

    @param rounded_rect
        The rounded rectangle to fill, in current region coordinate.

    @param color
        The color used to fill the rounded rectangle.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void FillRoundedRectangle(const RoundedRect& rounded_rect, const Color& color);
    
    /**
    Draws the outline of a rounded rectangle, using the brush, stroke width and stroke style in
    current state.

    @param rounded_rect
        The rounded rectangle to draw, in current region coordinate.
    */
    void DrawRoundedRectangle(const RoundedRect& rounded_rect) noexcept;

    /**
    Draws the outline of a rounded rectangle with the specified color, using the stroke width and
    stroke style in current state.

    @param rounded_rect
        The rounded rectangle to draw, in current region coordinate.

    @param color
        The color used to draw the rounded rectangle.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawRoundedRectangle(const RoundedRect& rounded_rect, const Color& color);

    /**
    Draws the outline of a rounded rectangle with the specified color and stroke width, using the
    stroke style in current state.

    @param rounded_rect
        The rounded rectangle to draw, in current region coordinate.

    @param color
        The color used to draw the rounded rectangle.

    @param stroke_width
        The width of stroke.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawRoundedRectangle(
        const RoundedRect& rounded_rect,
        const Color& color,
        float stroke_width);

    /**
    Fills an ellipse, using the brush in current state.

    @param ellipse
        The ellipse to fill, in current region coordinate.
    */
    void FillEllipse(const Ellipse& ellipse) noexcept;

    /**
    Fills an ellipse with the specified color.

    @param ellipse
        The ellipse to fill, in current region coordinate.

    @param color
        The color used to fill the ellipse.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void FillEllipse(const Ellipse& ellipse, const Color& color);
    
    /**
    Draws the outline of an ellipse, using the brush, stroke width and stroke style in current
    state.

    @param ellipse
        The ellipse to draw, in current region coordinate.
    */
    void DrawEllipse(const Ellipse& ellipse) noexcept;

    /**
    Draws the outline of an ellipse with the specified color, using the stroke width and stroke
    style in current state.

    @param ellipse
        The ellipse to draw, in current region coordinate.

    @param color
        The color used to draw the ellipse.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
    void DrawEllipse(const Ellipse& ellipse, const Color& color);

    /**
    Draws the outline of an ellipse with the specified color and stroke width, using the stroke
    style in current state.

    @param ellipse
        The ellipse to draw, in current region coordinate.

    @param color
        The color used to draw the ellipse.

    @param stroke_width
        The width of stroke.

    @throw zaf::COMError
        Thrown if creating brush for the specified color fails.
    */
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

    void DrawTextLayout(const dwrite::TextLayout& text_layout, const Point& position);
    void DrawTextLayout(
        const dwrite::TextLayout& text_layout,
        const Point& position, 
        const Color& color);

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
    friend class CanvasClipping;
    friend class CanvasRegion;
    friend class CanvasState;

    void PopRegion(CanvasClipping&& clipping_guard);
    void PopClipping(std::size_t tag);
    void PopState(std::size_t tag);

private:
    void PushInitialState();

    internal::CanvasRegionData CreateNewRegion(
        const Rect& region_rect,
        const Rect& paintable_rect) const noexcept;

    CanvasClipping InnerPushClipping(const Rect& clipping_rect) noexcept;

    const internal::CanvasStateData& CurrentState() const noexcept;

    template<typename T>
    T SnapToPixelsIfNeeded(const T& object, float stroke_width = 0) const noexcept;

    void InnerDrawLine(
        const Point& from_point,
        const Point& to_point,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style) noexcept;

    void InnerFillRectangle(const Rect& rect, const d2d::Brush& brush) noexcept;
    void InnerDrawRectangle(
        const Rect& rect,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style) noexcept;

    void InnerFillRoundedRectangle(
        const RoundedRect& rounded_rect,
        const d2d::Brush& brush) noexcept;
    void InnerDrawRoundedRectangle(
        const RoundedRect& rounded_rect,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style) noexcept;

    void InnerFillEllipse(const Ellipse& ellipse, const d2d::Brush& brush) noexcept;
    void InnerDrawEllipse(
        const Ellipse& ellipse,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style) noexcept;

    void InnerFillGeometry(const d2d::Geometry& geometry, const d2d::Brush& brush) noexcept;
    void InnerDrawGeometry(
        const d2d::Geometry& geometry,
        const d2d::Brush& brush,
        float stroke_width,
        const d2d::StrokeStyle& stroke_style);

    void InnerDrawTextFormat(
        const std::wstring& text,
        const dwrite::TextFormat& text_format,
        const Rect& rect,
        const d2d::Brush& brush) noexcept;

    void InnerDrawTextLayout(
        const dwrite::TextLayout& text_layout,
        const Point& position,
        const d2d::Brush& brush) noexcept;

private:
    d2d::Renderer renderer_;

    std::stack<internal::CanvasRegionData> regions_;
    std::size_t current_clipping_tag_{};

    std::stack<internal::CanvasStateData> states_;
    std::size_t current_state_tag_{};
};

}