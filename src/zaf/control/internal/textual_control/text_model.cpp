#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

void TextModel::SetText(std::wstring_view text) {
    SetTextInRange(text, Range{ 0, text_.length() });
}


void TextModel::SetTextInRange(std::wstring_view text, const Range& range) {

    ZAF_EXPECT(
        (range.index <= text_.length()) &&
        (range.EndIndex() <= text_.length()));

    text_.replace(range.index, range.length, text);

    RaiseTextChangedEvent(range, Range{ range.index, text.length() });
}

}