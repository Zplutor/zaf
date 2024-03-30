#include <zaf/control/textual/ranged_text_style.h>

namespace zaf::textual {

const Font* RangedTextStyle::GetFontAtIndex(std::size_t index) const {

    auto item = fonts_.FindItemContainsIndex(index);
    if (item) {
        return std::any_cast<Font>(&item->Value());
    }
    return nullptr;
}


const ColorPicker* RangedTextStyle::GetTextColorPickerAtIndex(std::size_t index) const {

    auto item = text_color_pickers_.FindItemContainsIndex(index);
    if (item) {
        return std::any_cast<ColorPicker>(&item->Value());
    }
    return nullptr;
}


const ColorPicker* RangedTextStyle::GetTextBackColorPickerAtIndex(std::size_t index) const {

    auto item = text_back_color_pickers_.FindItemContainsIndex(index);
    if (item) {
        return std::any_cast<ColorPicker>(&item->Value());
    }
    return nullptr;
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