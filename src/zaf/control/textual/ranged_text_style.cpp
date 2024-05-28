#include "ranged_text_style.h"
#include "ranged_text_style.h"
#include <zaf/control/textual/ranged_text_style.h>

namespace zaf::textual {

RangedTextStyle::RangedTextStyle(RangedTextStyle&& other) : 
    fonts_(std::move(other.fonts_)),
    text_colors_(std::move(other.text_colors_)),
    text_back_colors_(std::move(other.text_back_colors_)),
    inline_objects_(std::move(other.inline_objects_)) {

}


RangedTextStyle& RangedTextStyle::operator=(RangedTextStyle&& other) {

    this->fonts_ = std::move(other.fonts_);
    this->text_colors_ = std::move(other.text_colors_);
    this->text_back_colors_ = std::move(other.text_back_colors_);
    this->inline_objects_ = std::move(other.inline_objects_);
    return *this;
}


void RangedTextStyle::ReplaceSpan(const Range& span_range, std::size_t new_length) {

    fonts_.ReplaceSpan(span_range, new_length);
    text_colors_.ReplaceSpan(span_range, new_length);
    text_back_colors_.ReplaceSpan(span_range, new_length);
    inline_objects_.ReplaceSpan(span_range, new_length);
}


void RangedTextStyle::Clear() {
    fonts_.Clear();
    text_colors_.Clear();
    text_back_colors_.Clear();
    inline_objects_.Clear();
}


RangedTextStyle RangedTextStyle::Clone() const {

    RangedTextStyle result;
    result.fonts_ = this->fonts_;
    result.text_colors_ = this->text_colors_;
    result.text_back_colors_ = this->text_back_colors_;
    result.inline_objects_ = this->inline_objects_.Clone();
    return result;
}

}