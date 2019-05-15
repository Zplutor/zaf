#pragma once

#include <memory>
#include <stack>
#include <string>
#include <zaf/application.h>
#include <zaf/graphic/rect.h>
#include <zaf/graphic/clear_edge.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/renderer/renderer.h>
#include <zaf/graphic/brush/solid_color_brush.h>

namespace zaf {

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


class Canvas {
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
	Canvas(const Renderer& renderer, const Rect& renderer_rect, const Rect& paintable_rect);

	Renderer& GetRenderer() {
		return renderer_;
	}

    Rect GetAbsolutePaintableRect() const {
        return transformed_layers_.top().paintable_rect;
    }

	void SaveState();
	void RestoreState();

	ClearEdgeOption GetClearEdgeOption() const {
		return GetCurrentState()->clear_edge_option;
	}

	void SetClearEdgeOption(ClearEdgeOption option) {
		GetCurrentState()->clear_edge_option = option;
	}

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

	void DrawLine(const Point& from_point, const Point& to_point, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawLine(
            MakeClearEdgeForLine(from_point, stroke_width),
			MakeClearEdgeForLine(to_point, stroke_width), 
			state->brush, 
			stroke_width,
			state->stroke
		);
	}

	void DrawRectangle(const Rect& rect) {
		auto state = GetCurrentState();
		renderer_.DrawRectangle(MakeClearEdgeForFill(rect), state->brush);
	}

	void DrawRectangleFrame(const Rect& rect, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawRectangleFrame(
			MakeClearEdgeForLine(rect, stroke_width),
			state->brush,
			stroke_width, 
			state->stroke
		);
	}

    void DrawRoundedRectangle(const RoundedRect& rounded_rect) {
        auto state = GetCurrentState();
        renderer_.DrawRoundedRectangle(MakeClearEdgeForFill(rounded_rect), state->brush);
    }

    void DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width) {
        auto state = GetCurrentState();
        renderer_.DrawRoundedRectangleFrame(
            MakeClearEdgeForLine(rounded_rect, stroke_width),
            state->brush,
            stroke_width,
            state->stroke);
    }

	void DrawEllipse(const Ellipse& ellipse) {
		auto state = GetCurrentState();
		renderer_.DrawEllipse(MakeClearEdgeForFill(ellipse), state->brush);
	}

	void DrawEllipseFrame(const Ellipse& ellipse, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawEllipseFrame(
			MakeClearEdgeForLine(ellipse, stroke_width), 
			state->brush,
			stroke_width,
			state->stroke
		);
	}

	void DrawGeometry(const Geometry& geometry) {
		renderer_.DrawGeometry(geometry, GetCurrentState()->brush, Brush());
	}

	void DrawGeometryFrame(const Geometry& geometry, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawGeometryFrame(geometry, state->brush, stroke_width, state->stroke);
	}

	void DrawTextFormat(const std::wstring& text, const TextFormat& text_format, const Rect& rect) {
		renderer_.DrawTextFormat(text, text_format, rect, GetCurrentState()->brush);
	}

    void DrawTextLayout(const TextLayout& text_layout, const Point& position) {
        renderer_.DrawTextLayout(text_layout, position, GetCurrentState()->brush);
	}

    void DrawBitmap(const Bitmap& bitmap, const Rect& destination_rect, const DrawImageOptions& options = {}) {
        renderer_.DrawBitmap(bitmap, destination_rect, options.Opacity(), options.InterpolationMode(), options.SourceRect());
    }

    Point MakeClearEdgeForLine(const Point& point, float stroke_width) const;
    Point MakeClearEdgeForFill(const Point& point) const;

    Rect MakeClearEdgeForLine(const Rect& rect, float stroke_width) const;
    Rect MakeClearEdgeForFill(const Rect& rect) const;

    RoundedRect MakeClearEdgeForLine(const RoundedRect& rounded_rect, float stroke_width) const;
    RoundedRect MakeClearEdgeForFill(const RoundedRect& rounded_rect) const;

    Ellipse MakeClearEdgeForLine(const Ellipse& ellipse, float stroke_width) const;
    Ellipse MakeClearEdgeForFill(const Ellipse& ellipse) const;

    Canvas(Canvas&) = delete;
    Canvas& operator=(const Canvas&) = delete;

private:
	class State {
	public:
		State() : clear_edge_option(ClearEdgeOption::Clear) { } 

	public:
		ClearEdgeOption clear_edge_option;
		Brush brush;
		Stroke stroke;
        std::vector<Rect> clipping_rects;
	};

    class TransformedLayer {
    public:
        Rect rect;
        Rect paintable_rect;
    };

private:
	friend class Caret;
	friend class Control;
	friend class Window;

    void PushTransformLayer(const Rect& rect, const Rect& paintable_rect);
    void PopTransformLayer();

	void BeginPaint();
	void EndPaint();

private:
    void ApplyState(const std::shared_ptr<State>& state);
    void CancelState(const std::shared_ptr<State>& state);
    std::shared_ptr<State> GetCurrentState() const;

    Point AddAbsoluteOffset(const Point& point) const {
        return Point(point.x + transformed_rect_offset_.x, point.y + transformed_rect_offset_.y);
    }

    Rect AddAbsoluteOffset(const Rect& rect) const {
        return Rect(AddAbsoluteOffset(rect.position), rect.size);
    }

    RoundedRect AddAbsoluteOffset(const RoundedRect& rounded_rect) const {
        return RoundedRect(AddAbsoluteOffset(rounded_rect.rect), rounded_rect.x_radius, rounded_rect.y_radius);
    }

    Ellipse AddAbsoluteOffset(const Ellipse& ellipse) const {
        return Ellipse(AddAbsoluteOffset(ellipse.position), ellipse.x_radius, ellipse.y_radius);
    }

    void RemoveClearEdgeAbsoluteOffset(Point& point) const {
        point.x -= transformed_rect_clear_edge_offset_.x;
        point.y -= transformed_rect_clear_edge_offset_.y;
    }

    void RemoveClearEdgeAbsoluteOffset(Rect& rect) const {
        RemoveClearEdgeAbsoluteOffset(rect.position);
    }

    void RemoveClearEdgeAbsoluteOffset(RoundedRect& rounded_rect) const {
        RemoveClearEdgeAbsoluteOffset(rounded_rect.rect.position);
    }

    void RemoveClearEdgeAbsoluteOffset(Ellipse& ellipse) const {
        RemoveClearEdgeAbsoluteOffset(ellipse.position);
    }

private:
    Renderer renderer_;
    std::stack<TransformedLayer> transformed_layers_;
    
    Point transformed_rect_offset_;
    Point transformed_rect_clear_edge_offset_;
	
	std::vector<std::shared_ptr<State>> states_;
};

}