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

    ZAF_EXPECT(range.index <= text_.length());

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
    auto iterator = fonts.FindItemAtIndex(index);
    if (iterator != fonts.end()) {
        return iterator->Font();
    }
    return default_style_.Font();
}


const StyledText::RangedFontAccessor& StyledText::RangedFonts() const {
    return ranged_style_.Fonts();
}


StyledText::RangedFontAccessor& StyledText::RangedFonts() {
    return ranged_style_.Fonts();
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
    auto iterator = text_color_pickers.FindItemAtIndex(index);
    if (iterator != text_color_pickers.end()) {
        return iterator->ColorPicker();
    }
    return default_style_.TextColorPicker();
}


const StyledText::RangedColorPickerAccessor& StyledText::RangedTextColorPickers() const {
    return ranged_style_.TextColorPickers();
}


StyledText::RangedColorPickerAccessor& StyledText::RangedTextColorPickers() {
    return ranged_style_.TextColorPickers();
}


void StyledText::SetTextBackColorInRange(const Color& color, const Range& range) {
    SetTextBackColorPickerInRange(CreateColorPicker(color), range);
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
    auto iterator = text_back_color_pickers.FindItemAtIndex(index);
    if (iterator != text_back_color_pickers.end()) {
        return iterator->ColorPicker();
    }
    return default_style_.TextBackColorPicker();
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
    auto iterator = inline_objects.FindItemAtIndex(index);
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

    //Text
    auto slice_text = text_.substr(range.index, range.length);

    RangedTextStyle ranged_style;

    //Ranged fonts
    ranged_style_.Fonts().VisitItemsInRange(range, [&ranged_style](const auto& item) {
        ranged_style.SetFontInRange(item.Font(), item.Range());
    });

    //Ranged text color pickers
    ranged_style_.TextColorPickers().VisitItemsInRange(range, [&ranged_style](const auto& item) {
        ranged_style.SetTextColorPickerInRange(item.ColorPicker(), item.Range());
    });

    //Ranged background color pickers
    ranged_style_.TextBackColorPickers().VisitItemsInRange(
        range, 
        [&ranged_style](const auto& item) {
            ranged_style.SetTextBackColorPickerInRange(item.ColorPicker(), item.Range());
        });
        
    //Inline objects.
    ranged_style_.InlineObjects().VisitItemsInRange(range, [&ranged_style](const auto& item) {
        ranged_style.AttachInlineObjectToRange(item.Object()->Clone(), item.Range());
    });

    return StyledTextSlice{
        range.index,
        std::move(slice_text),
        default_style_,
        std::move(ranged_style)
    };
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


Range StyledText::SetSliceInRange(const StyledTextSlice& slice, const Range& range) {

    //Text
    auto new_range = SetTextInRange(slice.Text(), range);

    //Default style
    const auto& slice_default_style = slice.DefaultStyle();
    if (slice_default_style.Font() != this->DefaultFont()) {
        ranged_style_.SetFontInRange(slice_default_style.Font(), new_range);
    }

    //Default text color picker
    ranged_style_.SetTextColorPickerInRange(slice_default_style.TextColorPicker(), new_range);

    //Default text back color picker
    ranged_style_.SetTextBackColorPickerInRange(
        slice_default_style.TextBackColorPicker(), 
        new_range);

    const auto revise_item_range = [&slice, &new_range](const Range& item_range) {

        std::size_t index{};
        std::size_t length{};

        if (item_range.index < slice.Index()) {
            index = 0;
            length = item_range.length - (slice.Index() - item_range.index);
        }
        else {
            index = item_range.index - slice.Index();
            length = item_range.length;
        }

        Range revised_range{ index, length };
        if (revised_range.EndIndex() > slice.Text().length()) {
            revised_range.length -= revised_range.EndIndex() - slice.Text().length();
        }

        revised_range.index += new_range.index;
        return revised_range;
    };

    //Ranged fonts
    for (const auto& each_item : slice.RangedStyle().Fonts()) {
        ranged_style_.SetFontInRange(each_item.Font(), revise_item_range(each_item.Range()));
    }
    
    //Ranged text color picker
    for (const auto& each_item : slice.RangedStyle().TextColorPickers()) {
        ranged_style_.SetTextColorPickerInRange(
            each_item.ColorPicker(), 
            revise_item_range(each_item.Range()));
    }

    //Ranged text back color picker
    for (const auto& each_item : slice.RangedStyle().TextBackColorPickers()) {
        ranged_style_.SetTextBackColorPickerInRange(
            each_item.ColorPicker(),
            revise_item_range(each_item.Range()));
    }

    //Inline objects
    for (const auto& each_item : slice.RangedStyle().InlineObjects()) {
        ranged_style_.AttachInlineObjectToRange(
            each_item.Object()->Clone(),
            revise_item_range(each_item.Range()));
    }

    return new_range;
}


StyledText StyledText::GetSubText(const Range& range) const {

    ZAF_EXPECT(range.index <= text_.length());

    StyledText result;
    result.default_style_ = this->default_style_;

    result.SetText(text_.substr(range.index, range.length));

    const auto revise_item_range = [&range, &result](const Range& item_range) {

        std::size_t index{};
        std::size_t length{};

        if (item_range.index < range.index) {
            index = 0;
            length = item_range.length - (range.index - item_range.index);
        }
        else {
            index = item_range.index - range.index;
            length = item_range.length;
        }

        Range new_range{ index, length };
        if (new_range.EndIndex() > result.Text().length()) {
            new_range.length -= new_range.EndIndex() - result.Text().length();
        }
        return new_range;
    };
    
    //Fonts
    ranged_style_.Fonts().VisitItemsInRange(
        range, 
        [&revise_item_range, &result](const auto& item) {
            result.SetFontInRange(item.Font(), revise_item_range(item.Range()));
        });

    //Ranged text color pickers
    ranged_style_.TextColorPickers().VisitItemsInRange(
        range, 
        [&revise_item_range, &result](const auto& item) {
            result.SetTextColorPickerInRange(item.ColorPicker(), revise_item_range(item.Range()));
        });

    //Ranged background color pickers
    ranged_style_.TextBackColorPickers().VisitItemsInRange(
        range,
        [&revise_item_range, &result](const auto& item) {
            result.SetTextBackColorPickerInRange(
                item.ColorPicker(),
                revise_item_range(item.Range()));
        });

    //Inline objects.
    ranged_style_.InlineObjects().VisitItemsInRange(
        range, 
        [&revise_item_range, &result](const auto& item) {
            result.AttachInlineObjectToRange(
                item.Object()->Clone(),
                revise_item_range(item.Range()));
        });

    return result;
}


StyledText StyledText::Clone() const {

    StyledText result{ this->text_ };
    result.default_style_ = this->default_style_;
    result.ranged_style_ = this->ranged_style_.Clone();
    return result;
}


void StyledText::CheckRange(const Range& range) const {

    ZAF_EXPECT(
        (range.index <= text_.length()) &&
        (range.EndIndex() <= text_.length()));
}

}