#pragma once

#include <functional>
#include <memory>
#include <zaf/control/color_picker.h>

namespace zaf {

class Canvas;
class Rect;
class TextLayout;
class TextualControl;

namespace internal {

void PaintTextWithIcon(
	Canvas& canvas, 
    TextualControl& textual_control,
	const std::shared_ptr<TextLayout>& text_layout, 
	float icon_size,
	const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function);

const ColorPicker GetBoxBackgroundColorPicker();
const ColorPicker GetBoxBorderColorPicker();

}
}