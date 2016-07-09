#pragma once

#include <functional>
#include <memory>

namespace zaf {

class Canvas;
class Rect;
class TextLayout;
class TextualControl;

void PaintTextWithIcon(
	Canvas& canvas, 
    TextualControl& textual_control,
	const std::shared_ptr<TextLayout>& text_layout, 
	float icon_size,
	const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function);

}