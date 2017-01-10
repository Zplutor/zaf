#pragma once

#include <zaf/control/text_box.h>
#include <zaf/property/property_tag.h>

namespace zaf {

ZAF_DEFINE_PROPERTY_TAG(allowBeep, bool, TextBox, SetAllowBeep);
ZAF_DEFINE_PROPERTY_TAG(passwordCharacter, wchar_t, TextBox, SetPasswordCharacter);
ZAF_DEFINE_PROPERTY_TAG(inset, Frame, TextBox, SetInset);
ZAF_DEFINE_PROPERTY_TAG(isMultiline, bool, TextBox, SetIsMultiline);
ZAF_DEFINE_PROPERTY_TAG(isReadOnly, bool, TextBox, SetIsReadOnly);
ZAF_DEFINE_PROPERTY_TAG(maximumLength, std::uint32_t, TextBox, SetMaximumLength);
ZAF_DEFINE_PROPERTY_TAG(selectionRange, TextRange, TextBox, SetSelectionRange);
ZAF_DEFINE_PROPERTY_TAG(textValidator, TextValidator, TextBox, SetTextValidator);
ZAF_DEFINE_PROPERTY_TAG(usePasswordCharacter, bool, TextBox, SetUsePasswordCharacter);

ZAF_DEFINE_EVENT_PROPERTY_TAG(onSelectionChange, TextBox::SelectionChangeEvent, TextBox, GetSelectionChangeEvent);

}