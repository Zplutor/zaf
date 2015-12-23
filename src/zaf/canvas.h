#pragma once

#include <memory>
#include <string>
#include "color.h"
#include "rect.h"

namespace zaf {

class Font;
class Renderer;

class Canvas {
public:
	Canvas(const std::shared_ptr<Renderer>& renderer, 
		   const Rect& absolute_rect, 
		   const Rect& absolute_dirty_rect);

	const std::shared_ptr<Renderer>& GetRenderer() const {
		return renderer_;
	}

	const Rect& GetAbsoluteRect() const {
		return absolute_rect_;
	}

	const Rect& GetAbsoluteDirtyRect() const {
		return absolute_dirty_rect_;
	}

	void SetBrushColor(const Color& color);

	void DrawSingleLineText(const Rect& rect, const std::wstring& text, const Font& font);

	void DrawLine(const Point& from_point, const Point& to_point);
	void DrawRectangle(const Rect& rect);
	void DrawRectangleFrame(const Rect& rect, float strokeWidth);

private:
	friend class Caret;
	friend class Control;

	void BeginPaint();
	void EndPaint();

private:
	Canvas(Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;

private:
	std::shared_ptr<Renderer> renderer_;
	Rect absolute_rect_;
	Rect absolute_dirty_rect_;
};

}