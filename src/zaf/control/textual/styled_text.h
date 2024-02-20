#pragma once

#include <string>
#include <zaf/control/textual/text_style.h>

namespace zaf {

class StyledText {
public:
    StyledText(std::wstring text, TextStyle style) : 
        text_(std::move(text)), 
        style_(std::move(style)) {

    }

    const std::wstring& Text() const {
        return text_;
    }

    const TextStyle& Style() const {
        return style_;
    }

private:
    std::wstring text_;
    TextStyle style_;
};

}