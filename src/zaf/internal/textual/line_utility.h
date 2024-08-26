#pragma once

/**
@file
    Defines line utility functions for TextModel to handle multiline and line breaks.
*/

#include <functional>
#include <variant>
#include <zaf/control/textual/line_break.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::internal {

using RangeReplacedCallback = std::function<void(const Range& range, std::size_t new_length)>;

/**
@return
    True if the styled text is revised.
*/
bool ReviseLinesInStyledText(
    textual::StyledText& styled_text,
    bool supports_multiline,
    textual::LineBreak line_break,
    const RangeReplacedCallback& range_replaced_callback);

std::variant<std::reference_wrapper<const textual::StyledText>, textual::StyledText> 
    ReviseLinesInStyledTextView(
        const textual::StyledText& styled_text_view,
        bool supports_multiline,
        textual::LineBreak line_break);

void ReviseLinesInText(
    std::wstring& text, 
    bool supports_multiline, 
    textual::LineBreak line_break);

std::variant<std::wstring_view, std::wstring> ReviseLinesInTextView(
    std::wstring_view text_view, 
    bool supports_multiline,
    textual::LineBreak line_break);

}