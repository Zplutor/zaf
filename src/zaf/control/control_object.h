#pragma once

#include <zaf/control/control.h>
#include <zaf/control/control_parser.h>
#include <zaf/window/window.h>

namespace zaf {

ZAF_OBJECT_BEGIN(Control);
ZAF_OBJECT_PARSER(ControlParser);
ZAF_OBJECT_PROPERTY(Rect);
ZAF_OBJECT_PROPERTY(RectInWindow);
ZAF_OBJECT_PROPERTY(Position);
ZAF_OBJECT_PROPERTY(PositionInWindow);
ZAF_OBJECT_PROPERTY(X);
ZAF_OBJECT_PROPERTY(Y);
ZAF_OBJECT_PROPERTY(Size);
ZAF_OBJECT_PROPERTY(Width);
ZAF_OBJECT_PROPERTY(MinWidth);
ZAF_OBJECT_PROPERTY(MaxWidth);
ZAF_OBJECT_PROPERTY(FixedWidth);
ZAF_OBJECT_PROPERTY(FixedHeight);
ZAF_OBJECT_PROPERTY(FixedSize);
ZAF_OBJECT_PROPERTY(Height);
ZAF_OBJECT_PROPERTY(MinHeight);
ZAF_OBJECT_PROPERTY(MaxHeight);
ZAF_OBJECT_PROPERTY(AutoWidth);
ZAF_OBJECT_PROPERTY(AutoHeight);
ZAF_OBJECT_PROPERTY(AutoSize);
ZAF_OBJECT_PROPERTY(ContainsFocus);
ZAF_OBJECT_PROPERTY(ContainsMouse);
ZAF_OBJECT_PROPERTY(ContentRect);
ZAF_OBJECT_PROPERTY(ContentSize);
ZAF_OBJECT_PROPERTY(Name);
ZAF_OBJECT_PROPERTY(IsVisible);
ZAF_OBJECT_PROPERTY(IsEnabled);
ZAF_OBJECT_PROPERTY(IsMouseOver);
ZAF_OBJECT_PROPERTY(IsSelected);
ZAF_OBJECT_PROPERTY(CanFocused);
ZAF_OBJECT_PROPERTY(CanTabStop);
ZAF_OBJECT_PROPERTY(Margin);
ZAF_OBJECT_PROPERTY(Border);
ZAF_OBJECT_PROPERTY(Padding);
ZAF_OBJECT_PROPERTY(BackgroundColor);
ZAF_OBJECT_PROPERTY(BorderColor);
ZAF_OBJECT_PROPERTY(BackgroundImageLayout);
ZAF_OBJECT_PROPERTY(BackgroundImage);
ZAF_OBJECT_PROPERTY(Parent);
ZAF_OBJECT_PROPERTY(Window);
ZAF_OBJECT_PROPERTY(CanDoubleClick);
ZAF_OBJECT_PROPERTY(Layouter);
ZAF_OBJECT_PROPERTY(Tooltip);
ZAF_OBJECT_END;

}