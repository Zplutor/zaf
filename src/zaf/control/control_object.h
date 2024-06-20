#pragma once

#include <zaf/control/control.h>
#include <zaf/window/window.h>

namespace zaf {

ZAF_OBJECT_BEGIN(Control);
ZAF_PROPERTY(Rect);
ZAF_PROPERTY(Position);
ZAF_PROPERTY(X);
ZAF_PROPERTY(Y);
ZAF_PROPERTY(Size);
ZAF_PROPERTY(Width);
ZAF_PROPERTY(MinWidth);
ZAF_PROPERTY(MaxWidth);
ZAF_PROPERTY(FixedWidth);
ZAF_PROPERTY(FixedHeight);
ZAF_PROPERTY(FixedSize);
ZAF_PROPERTY(Height);
ZAF_PROPERTY(MinHeight);
ZAF_PROPERTY(MaxHeight);
ZAF_PROPERTY(AutoWidth);
ZAF_PROPERTY(AutoHeight);
ZAF_PROPERTY(AutoSize);
ZAF_PROPERTY(ContentRect);
ZAF_PROPERTY(ContentSize);
ZAF_PROPERTY(Name);
ZAF_PROPERTY(IsVisible);
ZAF_PROPERTY(IsEnabled);
ZAF_PROPERTY(CanFocused);
ZAF_PROPERTY(CanTabStop);
ZAF_PROPERTY(Margin);
ZAF_PROPERTY(Border);
ZAF_PROPERTY(Padding);
ZAF_PROPERTY(BackgroundColor);
ZAF_PROPERTY(BorderColor);
ZAF_PROPERTY(BackgroundImageLayout);
ZAF_PROPERTY(BackgroundImage);
ZAF_PROPERTY(Parent);
ZAF_PROPERTY(Window);
ZAF_PROPERTY(CanDoubleClick);
ZAF_PROPERTY(Layouter);
ZAF_PROPERTY(Style);
ZAF_PROPERTY(Tooltip);
ZAF_OBJECT_END;

}