#pragma once

#include <zaf/clipboard/data_object.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::internal {

void SaveStyledTextToClipboard(textual::StyledText styled_text);

bool LoadStyledTextFromClipboard(
    const clipboard::DataObject& data_object,
    textual::StyledText& styled_text,
    bool& is_styled_text);

}