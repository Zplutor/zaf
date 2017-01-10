#pragma once

#include <zaf/control/control.h>
#include <zaf/property/property_tag.h>

namespace zaf {

ZAF_DEFINE_PROPERTY_TAG(anchor, Anchor, Control, SetAnchor);
ZAF_DEFINE_PROPERTY_TAG(backgroundColor, Color, Control, SetBackgroundColor);
ZAF_DEFINE_PROPERTY_TAG(backgroundColorPicker, ColorPicker, Control, SetBackgroundColorPicker);
ZAF_DEFINE_PROPERTY_TAG(borderColor, Color, Control, SetBorderColor);
ZAF_DEFINE_PROPERTY_TAG(borderColorPicker, ColorPicker, Control, SetBorderColorPicker);
ZAF_DEFINE_PROPERTY_TAG(borderThickness, float, Control, SetBorderThickness);
ZAF_DEFINE_PROPERTY_TAG(child, std::shared_ptr<Control>, Control, AddChild);
ZAF_DEFINE_PROPERTY_TAG(layouter, Layouter, Control, SetLayouter);
ZAF_DEFINE_PROPERTY_TAG(height, float, Control, SetHeight);
ZAF_DEFINE_PROPERTY_TAG(maximumHeight, float, Control, SetMaximumHeight);
ZAF_DEFINE_PROPERTY_TAG(minimumHeight, float, Control, SetMinimumHeight);
ZAF_DEFINE_PROPERTY_TAG(maximumWidth, float, Control, SetMaximumWidth);
ZAF_DEFINE_PROPERTY_TAG(minimumWidth, float, Control, SetMinimumWidth);
ZAF_DEFINE_PROPERTY_TAG(position, Point, Control, SetPosition);
ZAF_DEFINE_PROPERTY_TAG(rect, Rect, Control, SetRect);
ZAF_DEFINE_PROPERTY_TAG(size, Size, Control, SetSize);
ZAF_DEFINE_PROPERTY_TAG(width, float, Control, SetWidth);

}