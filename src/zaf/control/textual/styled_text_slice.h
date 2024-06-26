#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/textual/ranged_text_style.h>

namespace zaf::textual {

class StyledTextSlice : NonCopyable {
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
    RangedTextStyle ranged_style_;
};

}