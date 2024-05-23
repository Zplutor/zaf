#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/textual/default_text_style.h>
#include <zaf/control/textual/ranged_text_style.h>

namespace zaf::textual {

class StyledText;

class StyledTextSlice : NonCopyable {
public:
    StyledTextSlice(
        std::size_t slice_index,
        std::wstring text,
        DefaultTextStyle default_style,
        RangedTextStyle ranged_style) noexcept
        :
        slice_index_(slice_index),
        text_(std::move(text)),
        default_style_(std::move(default_style)),
        ranged_style_(std::move(ranged_style)) {

    }

    std::size_t Index() const noexcept {
        return slice_index_;
    }

    const std::wstring& Text() const noexcept {
        return text_;
    }

    const DefaultTextStyle& DefaultStyle() const noexcept {
        return default_style_;
    }

    const RangedTextStyle& RangedStyle() const noexcept {
        return ranged_style_;
    }

private:
    std::size_t slice_index_{};
    std::wstring text_;
    DefaultTextStyle default_style_;
    RangedTextStyle ranged_style_;
};

}