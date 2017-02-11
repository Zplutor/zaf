#pragma once

#include <functional>
#include <string>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/font/font.h>
#include <zaf/property/control_property_tag.h>

namespace zaf {

ZAF_DEFINE_PROPERTY_TAG(defaultFont, Font, TextualControl, SetDefaultFont);
ZAF_DEFINE_PROPERTY_TAG(paragraphAlignment, ParagraphAlignment, TextualControl, SetParagraphAlignment);
ZAF_DEFINE_PROPERTY_TAG(text, std::wstring, TextualControl, SetText);
ZAF_DEFINE_PROPERTY_TAG(textAlignment, TextAlignment, TextualControl, SetTextAlignment);
ZAF_DEFINE_PROPERTY_TAG(defaultTextColor, Color, TextualControl, SetDefaultTextColor);
ZAF_DEFINE_PROPERTY_TAG(defaultTextColorPicker, ColorPicker, TextualControl, SetDefaultTextColorPicker);
ZAF_DEFINE_PROPERTY_TAG(wordWrapping, WordWrapping, TextualControl, SetWordWrapping);

ZAF_DEFINE_EVENT_PROPERTY_TAG(onTextChange, TextualControl::TextChangeEvent, TextualControl, GetTextChangeEvent);

}