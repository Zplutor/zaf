#include "ranged_text_style.h"
#include "ranged_text_style.h"
#include <zaf/control/textual/ranged_text_style.h>

namespace zaf::textual {

RangedTextStyle::RangedTextStyle(RangedTextStyle&& other) : 
    fonts_(std::move(other.fonts_)),
    text_color_pickers_(std::move(other.text_color_pickers_)),
    text_back_color_pickers_(std::move(other.text_back_color_pickers_)),
    inline_objects_(std::move(other.inline_objects_)) {

}


RangedTextStyle& RangedTextStyle::operator=(RangedTextStyle&& other) {

    this->fonts_ = std::move(other.fonts_);
    this->text_color_pickers_ = std::move(other.text_color_pickers_);
    this->text_back_color_pickers_ = std::move(other.text_back_color_pickers_);
    this->inline_objects_ = std::move(other.inline_objects_);
    return *this;
}


void RangedTextStyle::ReplaceSpan(const Range& span_range, std::size_t new_length) {

    fonts_.ReplaceSpan(span_range, new_length);
    text_color_pickers_.ReplaceSpan(span_range, new_length);
    text_back_color_pickers_.ReplaceSpan(span_range, new_length);
    inline_objects_.ReplaceSpan(span_range, new_length);
}


void RangedTextStyle::Clear() {
    fonts_.Clear();
    text_color_pickers_.Clear();
    text_back_color_pickers_.Clear();
    inline_objects_.Clear();
}


RangedTextStyle RangedTextStyle::Clone() const {

    RangedTextStyle result;
    result.fonts_ = this->fonts_;
    result.text_color_pickers_ = this->text_color_pickers_;
    result.text_back_color_pickers_ = this->text_back_color_pickers_;
    result.inline_objects_ = this->inline_objects_.Clone();
    return result;
}

}