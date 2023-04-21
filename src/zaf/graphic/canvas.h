#pragma once

#include <memory>
#include <stack>
#include <string>
#include <zaf/graphic/brush/solid_color_brush.h>
#include <zaf/graphic/canvas_layer_guard.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/internal/alignment_helper.h>
#include <zaf/graphic/internal/canvas_layer.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/renderer/renderer.h>

namespace zaf {

class EllipseGeometry;
class PathGeometry;
class RectangleGeometry;
class RoundedRectangleGeometry;

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


class Canvas : NonCopyable {
public:
    class StateGuard {
    public:
        StateGuard(Canvas& canvas) : canvas_(canvas) {
            canvas_.SaveState();
        }

        ~StateGuard() {
            canvas_.RestoreState();
        }

    private:
        StateGuard(StateGuard&) = delete;
        StateGuard& operator=(const StateGuard&) = delete;

    private:
        Canvas& canvas_;
    };

public:
    /**
    Constructs a canvas with specified renderer and rects.

    @param renderer
        The renderer to which the canvas paints to.

    @param canvas_rect
        The rectangle area of canvas in renderer.

    @param paintable_rect
        The rectangle area that canvas can paint in renderer. It is a sub area of canvas_rect.
    */
    Canvas(Renderer& renderer, const Rect& canvas_rect, const Rect& paintable_rect);

    Renderer& GetRenderer() {
        return renderer_;
    }

    Rect GetAbsolutePaintableRect() const {
        return layers_.top().paintable_rect;
    }

    /**
    Pushes a new layer into canvas.

    @param layer_rect
        The rectangle area of the new layer, in current layer coordinate.

    @param paintable_rect
        The rectangle area that can be painted in the new layer, in current layer coordinate. It is
        a sub area of layer_rect.
    */
    [[nodiscard]]
    CanvasLayerGuard PushLayer(const Rect& layer_rect, const Rect& paintable_rect);

    void SaveState();
    void RestoreState();

    void SetBrush(const Brush& brush) {
        GetCurrentState()->brush = brush;
    }

    void SetBrushWithColor(const Color& color) {
        auto brush = renderer_.CreateSolidColorBrush(color);
        SetBrush(brush);
    }

    void SetStroke(const Stroke& stroke) {
        GetCurrentState()->stroke = stroke;
    }

    void PushClippingRect(const Rect& rect);
    void PopClippingRect();

    void Clear();

    void DrawLine(const Point& from_point, const Point& to_point, float stroke_width);

    void DrawRectangle(const Rect& rect);
    void DrawRectangleFrame(const Rect& rect, float stroke_width);

    void DrawRoundedRectangle(const RoundedRect& rounded_rect);
    void DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width);

    void DrawEllipse(const Ellipse& ellipse);
    void DrawEllipseFrame(const Ellipse& ellipse, float stroke_width);

    void DrawGeometry(const Geometry& geometry);
    void DrawGeometryFrame(const Geometry& geometry, float stroke_width);

    void DrawTextFormat(const std::wstring& text, const TextFormat& text_format, const Rect& rect);
    void DrawTextLayout(const TextLayout& text_layout, const Point& position);

    void DrawBitmap(
        const RenderBitmap& bitmap,
        const Rect& destination_rect, 
        const DrawImageOptions& options = {});

    PathGeometry CreatePathGeometry() const;
    RectangleGeometry CreateRectangleGeometry(const Rect& rect) const;
    RoundedRectangleGeometry CreateRoundedRectangleGeometry(const RoundedRect& rounded_rect) const;
    EllipseGeometry CreateEllipseGeometry(const Ellipse& ellipse) const;

private:
    class State {
    public:
        Brush brush;
        Stroke stroke;
        std::vector<Rect> clipping_rects;
    };

private:
    friend class Control;
    friend class Window;
    friend class TextBox;
    friend class CanvasLayerGuard;

    void PushTransformLayer(const Rect& rect, const Rect& paintable_rect);
    void PopTransformLayer();

    void BeginPaint();
    void EndPaint();

    void PopLayer();

private:
    internal::CanvasLayer CreateNewLayer(const Rect& layer_rect, const Rect& paintable_rect) const;

    void ApplyState(const std::shared_ptr<State>& state);
    void CancelState(const std::shared_ptr<State>& state);
    std::shared_ptr<State> GetCurrentState() const;

    template<typename T>
    T AlignWithTransformLayer(const T& object, float stroke_width = 0) const {

        const auto& current_layer = layers_.top();
        return internal::AlignInRelatedCoordinateSystem(
            object, 
            stroke_width, 
            current_layer.rect.position, 
            current_layer.aligned_rect.position);
    }

private:
    Renderer renderer_;
    std::stack<internal::CanvasLayer> layers_;
    std::vector<std::shared_ptr<State>> states_;
};

}