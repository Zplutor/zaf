#pragma once

#include <memory>
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
	Canvas(const Renderer& renderer);

	Renderer& GetRenderer() {
		return renderer_;
	}

	const Rect& GetAbsoluteRect() const {
		return absolute_rect_;
	}

	const Rect& GetAbsolutePaintableRect() const {
		return absolute_paintable_rect_;
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

	void DrawLine(const Point& from_point, const Point& to_point, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawLine(
            MakeClearEdgePointForLine(from_point, stroke_width, state->clear_edge_option),
			MakeClearEdgePointForLine(to_point, stroke_width, state->clear_edge_option), 
			state->brush, 
			stroke_width,
			state->stroke
		);
	}

	void DrawRectangle(const Rect& rect) {
		auto state = GetCurrentState();
		renderer_.DrawRectangle(MakeClearEdgeRectForFill(rect, state->clear_edge_option), state->brush);
	}

	void DrawRectangleFrame(const Rect& rect, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawRectangleFrame(
			MakeClearEdgeRectForLine(rect, stroke_width, state->clear_edge_option),
			state->brush,
			stroke_width, 
			state->stroke
		);
	}

    void DrawRoundedRectangle(const RoundedRect& rounded_rect) {
        auto state = GetCurrentState();
        renderer_.DrawRoundedRectangle(MakeClearEdgeRoundedRectForFill(rounded_rect, state->clear_edge_option), state->brush);
    }

    void DrawRoundedRectangleFrame(const RoundedRect& rounded_rect, float stroke_width) {
        auto state = GetCurrentState();
        renderer_.DrawRoundedRectangleFrame(
            MakeClearEdgeRoundedRectForLine(rounded_rect, stroke_width, state->clear_edge_option),
            state->brush,
            stroke_width,
            state->stroke);
    }

	void DrawEllipse(const Ellipse& ellipse) {
		auto state = GetCurrentState();
		renderer_.DrawEllipse(MakeClearEdgeEllipseForFill(ellipse, state->clear_edge_option), state->brush);
	}

	void DrawEllipseFrame(const Ellipse& ellipse, float stroke_width) {
		auto state = GetCurrentState();
		renderer_.DrawEllipseFrame(
			MakeClearEdgeEllipseForLine(ellipse, stroke_width, state->clear_edge_option), 
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

	void DrawText(const std::wstring& text, const TextFormat& text_format, const Rect& rect) {
		renderer_.DrawText(text, text_format, rect, GetCurrentState()->brush);
	}

	void DrawText(const TextLayout& text_layout, const Point& position) {
		renderer_.DrawText(text_layout, position, GetCurrentState()->brush);
	}

    void DrawBitmap(const Bitmap& bitmap, const Rect& rect) {
        DrawBitmap(bitmap, rect, DrawImageOptions());
    }

    void DrawBitmap(const Bitmap& bitmap, const Rect& rect, const DrawImageOptions& options) {
        renderer_.DrawBitmap(bitmap, rect, options.Opacity(), options.InterpolationMode(), options.SourceRect());
    }

private:
	class State {
	public:
		State() : clear_edge_option(ClearEdgeOption::Clear) { } 

	public:
		ClearEdgeOption clear_edge_option;
		Brush brush;
		Stroke stroke;
	};

private:
	friend class Caret;
	friend class Control;
	friend class Window;

	void SetRects(const Rect& absolute_rect, const Rect& absolute_paintable_rect) {
		absolute_rect_ = absolute_rect;
		absolute_paintable_rect_ = absolute_paintable_rect;
	}

	void BeginPaint();
	void EndPaint();

private:
	std::shared_ptr<State> GetCurrentState() const;

	Canvas(Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;

private:
	Rect absolute_rect_;
	Rect absolute_paintable_rect_;

	Renderer renderer_;
	Layer layer_;
	
	std::vector<std::shared_ptr<State>> states_;
};

}