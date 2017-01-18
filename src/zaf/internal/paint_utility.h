#pragma once

#include <functional>
#include <memory>
#include <zaf/control/color_picker.h>

namespace zaf {

class Canvas;
class Rect;
class Stroke;
class TextLayout;
class TextualControl;

namespace internal {

void DrawTextWithIcon(
	Canvas& canvas, 
    TextualControl& textual_control,
	TextLayout& text_layout, 
	float icon_size,
	const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function);

void DrawFocusRectangleFrame(Canvas& canvas, const Rect& rect);

const ColorPicker GetBoxBackgroundColorPicker();
const ColorPicker GetBoxBorderColorPicker();

}
}