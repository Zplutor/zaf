#pragma once

#include <memory>
#include <string>
#include <zaf/application.h>
#include <zaf/base/rect.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/graphic/brush/solid_color_brush.h>

namespace zaf {

class Font;
class Layer;

class Canvas {
public:
	Canvas(const std::shared_ptr<Renderer>& renderer);

	const std::shared_ptr<Renderer>& GetRenderer() const {
		return renderer_;
	}

	const Rect& GetAbsoluteRect() const {
		return absolute_rect_;
	}

	const Rect& GetAbsolutePaintableRect() const {
		return absolute_paintable_rect_;
	}

	const std::shared_ptr<Brush> CreateSolidColorBrush(const Color& color) {
		return renderer_->CreateSolidColorBrush(color);
	}

	const std::shared_ptr<Stroke> CreateStroke(const Stroke::Properties& properties) {
		return Application::GetInstance().GetRendererFactory()->CreateStroke(properties);
	}

	void SetBrush(const std::shared_ptr<Brush>& brush) {
		set_brush_ = brush;
	}

	void SetBrushWithColor(const Color& color) {
		solid_color_brush_->SetColor(color);
		set_brush_ = solid_color_brush_;
	}

	void SetStroke(const std::shared_ptr<Stroke>& stroke) {
		set_stroke_ = stroke;
	}

	void DrawSingleLineText(const Rect& rect, const std::wstring& text, const Font& font);

	void DrawLine(const Point& from_point, const Point& to_point, float stroke_width) {
		renderer_->DrawLine(from_point, to_point, set_brush_, ToRenderStrokeWidth(stroke_width), set_stroke_);
	}

	void DrawRectangle(const Rect& rect) {
		renderer_->DrawRectangle(rect, set_brush_);
	}

	void DrawRectangleFrame(const Rect& rect, float stroke_width) {
		renderer_->DrawRectangleFrame(rect, set_brush_, ToRenderStrokeWidth(stroke_width), set_stroke_);
	}

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
	static float ToRenderStrokeWidth(float stroke_width) {
		return stroke_width * 2;
	}

	Canvas(Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;

private:
	Rect absolute_rect_;
	Rect absolute_paintable_rect_;

	std::shared_ptr<Renderer> renderer_;
	std::shared_ptr<Layer> layer_;
	std::shared_ptr<Brush> set_brush_;
	std::shared_ptr<Stroke> set_stroke_;
	std::shared_ptr<SolidColorBrush> solid_color_brush_;
};

}