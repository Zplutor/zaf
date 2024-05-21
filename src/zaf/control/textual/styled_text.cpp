#include <zaf/control/textual/styled_text.h>
#include <zaf/base/as.h>

namespace zaf::textual {

StyledText::StyledText() {

}


StyledText::StyledText(std::wstring text) : text_(std::move(text)) {

}


void StyledText::SetText(std::wstring text) {

    text_ = std::move(text);
    ranged_style_.Clear();
}


Range StyledText::SetTextInRange(std::wstring_view text, const Range& range) {

    CheckRange(range);

    text_.replace(range.index, range.length, text);
    ranged_style_.ReplaceSpan(range, text.length());

    return Range{ range.index, text.length() };
}


Range StyledText::AppendText(std::wstring_view text) {
    return SetTextInRange(text, Range{ text_.length(), 0 });
}


Range StyledText::AppendText(std::wstring_view text, const TextStyle& style) {

    auto new_range = AppendText(text);

    if (style.Font()) {
        ranged_style_.SetFontInRange(*style.Font(), new_range);
    }

    if (style.TextColorPicker()) {
        ranged_style_.SetTextColorPickerInRange(style.TextColorPicker(), new_range);
    }

    if (style.TextBackColorPicker()) {
        ranged_style_.SetTextBackColorPickerInRange(style.TextBackColorPicker(), new_range);
    }

    if (style.InlineObject()) {
        ranged_style_.AttachInlineObjectToRange(style.InlineObject(), new_range);
    }

    return new_range;
}


void StyledText::ClearText() {
    SetText({});
}


void StyledText::SetFontInRange(Font font, const Range& range) {

    CheckRange(range);
    ranged_style_.SetFontInRange(std::move(font), range);
}


void StyledText::ClearRangedFonts() {
    ranged_style_.ClearFonts();
}


const Font& StyledText::GetFontAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto& fonts = ranged_style_.Fonts();
    auto iterator = fonts.FindItemContainsIndex(index);
    if (iterator != fonts.end()) {
        return iterator->Font();
    }
    return default_font_;
}


const StyledText::RangedFontAccessor& StyledText::RangedFonts() const {
    return ranged_style_.Fonts();
}


StyledText::RangedFontAccessor& StyledText::RangedFonts() {
    return ranged_style_.Fonts();
}


void StyledText::SetDefaultTextColor(const Color& color) {
    SetDefaultTextColorPicker(CreateColorPicker(color));
}


void StyledText::SetDefaultTextColorPicker(ColorPicker color_picker) {
    ZAF_EXPECT(color_picker);
    default_text_color_picker_ = std::move(color_picker);
}


void StyledText::SetTextColorInRange(const Color& color, const Range& range) {
    SetTextColorPickerInRange(CreateColorPicker(color), range);
}


void StyledText::SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {

    CheckRange(range);
    ranged_style_.SetTextColorPickerInRange(std::move(color_picker), range);
}


void StyledText::ClearRangedTextColorPickers() {
    ranged_style_.ClearTextColorPickers();
}


const ColorPicker& StyledText::GetTextColorPickerAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto& text_color_pickers = ranged_style_.TextColorPickers();
    auto iterator = text_color_pickers.FindItemContainsIndex(index);
    if (iterator != text_color_pickers.end()) {
        return iterator->ColorPicker();
    }
    return default_text_color_picker_;
}


const StyledText::RangedColorPickerAccessor& StyledText::RangedTextColorPicker() const {
    return ranged_style_.TextColorPickers();
}


StyledText::RangedColorPickerAccessor& StyledText::RangedTextColorPicker() {
    return ranged_style_.TextColorPickers();
}


void StyledText::SetDefaultTextBackColor(const Color& color) {
    SetDefaultTextBackColorPicker(CreateColorPicker(color));
}


void StyledText::SetDefaultTextBackColorPicker(ColorPicker color_picker) {
    ZAF_EXPECT(color_picker);
    default_text_back_color_picker_ = std::move(color_picker);
}


void StyledText::SetTextBackColorPickerInRange(ColorPicker color_picker, const Range& range) {

    CheckRange(range);
    ranged_style_.SetTextBackColorPickerInRange(std::move(color_picker), range);
}


void StyledText::ClearRangedTextBackColorPickers() {
    ranged_style_.ClearTextBackColorPickers();
}


const ColorPicker& StyledText::GetTextBackColorPickerAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto& text_back_color_pickers = ranged_style_.TextBackColorPickers();
    auto iterator = text_back_color_pickers.FindItemContainsIndex(index);
    if (iterator != text_back_color_pickers.end()) {
        return iterator->ColorPicker();
    }
    return default_text_back_color_picker_;
}


const StyledText::RangedColorPickerAccessor& StyledText::RangedTextBackColorPickers() const {
    return ranged_style_.TextBackColorPickers();
}


StyledText::RangedColorPickerAccessor& StyledText::RangedTextBackColorPickers() {
    return ranged_style_.TextBackColorPickers();
}


void StyledText::AttachInlineObjectToRange(
    std::shared_ptr<textual::InlineObject> object,
    const Range& range) {

    CheckRange(range);
    ranged_style_.AttachInlineObjectToRange(std::move(object), range);
}


void StyledText::ClearInlineObjects() {
    ranged_style_.ClearInlineObjects();
}


std::shared_ptr<textual::InlineObject> StyledText::GetInlineObjectAtIndex(
    std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto& inline_objects = ranged_style_.InlineObjects();
    auto iterator = inline_objects.FindItemContainsIndex(index);
    if (iterator != inline_objects.end()) {
        return iterator->Object();
    }
    return nullptr;
}


const InlineObjectAccessor& StyledText::InlineObjects() const {
    return ranged_style_.InlineObjects();
}


StyledTextSlice StyledText::Slice(const Range& range) const {

    ZAF_EXPECT(range.index <= text_.length());

    if (range.length == 0) {
        return StyledTextSlice{ range.index, {}, {} };
    }

    //Text
    auto slice_text = text_.substr(range.index, range.length);

    RangedTextStyle slice_style;

    //Ranged fonts
    for (const auto& each_item : ranged_style_.Fonts()) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        slice_style.SetFontInRange(each_item.Font(), each_item.Range());
    }

    //Ranged text color pickers
    for (const auto& each_item : ranged_style_.TextColorPickers()) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        slice_style.SetTextColorPickerInRange(each_item.ColorPicker(), each_item.Range());
    }

    //Ranged background color pickers
    for (const auto& each_item : ranged_style_.TextBackColorPickers()) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        slice_style.SetTextBackColorPickerInRange(each_item.ColorPicker(), each_item.Range());
    }

    //Inline objects.
    for (const auto& each_item : ranged_style_.InlineObjects()) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        if (range.Contains(each_item.Range())) {
            slice_style.AttachInlineObjectToRange(
                each_item.Object()->Clone(),
                each_item.Range());
        }
    }

    return StyledTextSlice{ range.index, std::move(slice_text), std::move(slice_style) };
}


void StyledText::ReplaceSlice(const Range& slice_range, const StyledTextSlice& new_slice) {

    CheckRange(slice_range);
    ZAF_EXPECT(slice_range.index == new_slice.Index());

    SetTextInRange(new_slice.Text(), slice_range);

    const auto& ranged_style = new_slice.RangedStyle();
    for (const auto& each_item : ranged_style.Fonts()) {
        ranged_style_.SetFontInRange(each_item.Font(), each_item.Range());
    }

    for (const auto& each_item : ranged_style.TextColorPickers()) {
        ranged_style_.SetTextColorPickerInRange(each_item.ColorPicker(), each_item.Range());
    }

    for (const auto& each_item : ranged_style.TextBackColorPickers()) {
        ranged_style_.SetTextBackColorPickerInRange(each_item.ColorPicker(), each_item.Range());
    }

    for (const auto& each_item : ranged_style.InlineObjects()) {
        ranged_style_.AttachInlineObjectToRange(
            each_item.Object()->Clone(),
            each_item.Range());
    }
}


StyledText StyledText::SubText(const Range& sub_range) const {

    ZAF_EXPECT(sub_range.index <= text_.length());

    StyledText result;
    result.SetDefaultFont(default_font_);
    result.SetDefaultTextColorPicker(default_text_color_picker_);
    result.SetDefaultTextBackColorPicker(default_text_back_color_picker_);

    auto slice = this->Slice(sub_range);

    result.SetText(slice.Text());

    const auto revise_item_range = [&sub_range, &result](const Range& item_range) {

        std::size_t index{};
        std::size_t length{};

        if (item_range.index < sub_range.index) {
            index = 0;
            length = item_range.length - (sub_range.index - item_range.index);
        }
        else {
            index = item_range.index - sub_range.index;
            length = item_range.length;
        }

        Range new_range{ index, length };
        if (new_range.EndIndex() > result.Text().length()) {
            new_range.length -= new_range.EndIndex() - result.Text().length();
        }
        return new_range;
    };
    
    //Fonts
    for (const auto& each_item : slice.RangedStyle().Fonts()) {
        result.SetFontInRange(each_item.Font(), revise_item_range(each_item.Range()));
    }

    //Text color pickers
    for (const auto& each_item : slice.RangedStyle().TextColorPickers()) {
        result.SetTextColorPickerInRange(
            each_item.ColorPicker(), 
            revise_item_range(each_item.Range()));
    }

    //Text back color pickers
    for (const auto& each_item : slice.RangedStyle().TextBackColorPickers()) {
        result.SetTextBackColorPickerInRange(
            each_item.ColorPicker(),
            revise_item_range(each_item.Range()));
    }

    //Inline objects
    for (const auto& each_item : slice.RangedStyle().InlineObjects()) {
        result.AttachInlineObjectToRange(each_item.Object(), revise_item_range(each_item.Range()));
    }

    return result;
}


StyledText StyledText::Clone() const {

    StyledText result{ this->text_ };
    result.default_font_ = this->default_font_;
    result.default_text_color_picker_ = this->default_text_color_picker_;
    result.default_text_back_color_picker_ = this->default_text_back_color_picker_;
    result.ranged_style_ = this->ranged_style_.Clone();
    return result;
}


void StyledText::CheckRange(const Range& range) const {

    ZAF_EXPECT(
        (range.index <= text_.length()) &&
        (range.EndIndex() <= text_.length()));
}

}