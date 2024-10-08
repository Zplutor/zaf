#pragma once

#include <functional>
#include <memory>
#include <zaf/graphic/color.h>
#include <zaf/graphic/size.h>

namespace zaf {
namespace dwrite {
class TextLayout;
}

class Canvas;
class ClickableControl;
class Rect;
class TextualControl;

namespace internal {

constexpr float IconSize = 12;
constexpr float IconMargin = 5;

void DrawTextWithIcon(
    Canvas& canvas, 
    const TextualControl& textual_control,
    dwrite::TextLayout& text_layout,
    const std::function<void(Canvas& canvas, const Rect& icon_rect)>& paint_icon_function);

void DrawFocusRectangleFrame(Canvas& canvas, const Rect& rect);

Color GetBoxBackgroundColor(const ClickableControl& control);
Color GetBoxBorderColor(const ClickableControl& control);

}
}