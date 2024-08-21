#pragma once

#include <zaf/control/textual/styled_text.h>

namespace zaf::internal {

void SaveStyledTextToClipboard(textual::StyledText styled_text);

bool LoadStyledTextFromClipboard(textual::StyledText& styled_text, bool& is_styled_text);

}