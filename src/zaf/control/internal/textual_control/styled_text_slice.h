#pragma once

#include <zaf/control/internal/textual_control/ranged_text_style.h>

namespace zaf::internal {

class StyledTextSlice {
public:
    StyledTextSlice(
        std::size_t slice_index,
        std::wstring text,
        RangedTextStyle ranged_style)
        :
        slice_index_(slice_index),
        text_(std::move(text)),
        ranged_style_(std::move(ranged_style)) {

    }

    std::size_t Index() const {
        return slice_index_;
    }

    const std::wstring& Text() const {
        return text_;
    }

    const RangedTextStyle& RangedStyle() const {
        return ranged_style_;
    }

private:
    std::size_t slice_index_{};
    std::wstring text_;
    internal::RangedTextStyle ranged_style_;
};

}