#pragma once

#include <functional>
#include <memory>
#include <zaf/control/color_picker.h>
#include <zaf/graphic/size.h>

namespace zaf {

class Canvas;
class ClickableControl;
class Rect;
class Stroke;
class TextLayout;
class TextualControl;

namespace internal {

constexpr float IconSize = 12;
constexpr float IconMargin = 5;

void DrawTextWithIcon(
    Canvas& canvas, 
    TextualControl& textual_control,
    TextLayout& text_layout, 
    const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function);

void DrawFocusRectangleFrame(Canvas& canvas, const Rect& rect);

Color GetBoxBackgroundColor(const ClickableControl& control);
Color GetBoxBorderColor(const ClickableControl& control);

}
}