#pragma once

#include <functional>
#include <zaf/control/textual/line_break.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::internal {

/**
Line utility functions for TextModel to handle multiline and line breaks.
*/
class LineUtility {
public:
    static void ReviseLinesInStyledText(
        textual::StyledText& styled_text,
        bool supports_multiline,
        textual::LineBreak line_break);

    static void ReviseLinesInText(
        std::wstring& text, 
        bool supports_multiline, 
        textual::LineBreak line_break);

private:
    LineUtility() = delete;
};

}