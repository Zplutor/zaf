#pragma once

#include <functional>
#include <memory>

namespace zaf {

class Canvas;
class Control;
class Rect;
class TextLayout;

void PaintTextWithIcon(
	Canvas& canvas, 
	const Rect& paint_rect,
	const std::shared_ptr<TextLayout>& text_layout, 
	float icon_size,
	const std::function<void(Canvas& canvas, const Rect& text_rect, const std::shared_ptr<TextLayout>& text_layout)>& paint_text_function,
	const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function
);

}