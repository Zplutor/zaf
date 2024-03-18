#include <zaf/internal/textual/styled_text.h>
#include <zaf/base/as.h>

namespace zaf::internal {

StyledText::StyledText() {

}


StyledText::StyledText(std::wstring text) : text_(std::move(text)) {

}


void StyledText::SetText(std::wstring text) {

    text_ = std::move(text);
    ranged_style_.Clear();
}


void StyledText::SetTextInRange(std::wstring_view text, const Range& range) {

    CheckRange(range);
    text_.replace(range.index, range.length, text);
    ranged_style_.ReplaceSpan(range, text.length());
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

    auto font = ranged_style_.GetFontAtIndex(index);
    if (font) {
        return *font;
    }
    return default_font_;
}


StyledText::RangedFontEnumerator StyledText::RangedFonts() const {
    return ranged_style_.Fonts();
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

    auto picker = ranged_style_.GetTextColorPickerAtIndex(index);
    if (picker) {
        return *picker;
    }
    return default_text_color_picker_;
}


StyledText::RangedColorPickerEnumerator StyledText::RangedTextColorPicker() const {
    return ranged_style_.TextColorPickers();
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

    auto picker = ranged_style_.GetTextBackColorPickerAtIndex(index);
    if (picker) {
        return *picker;
    }
    return default_text_back_color_picker_;
}


StyledText::RangedColorPickerEnumerator StyledText::RangedTextBackColorPickers() const {
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

    auto object = ranged_style_.GetInlineObjectAtIndex(index);
    if (object) {
        return object;
    }
    return nullptr;
}


StyledText::InlineObjectEnumerator StyledText::InlineObjects() const {
    return ranged_style_.InlineObjects();
}


StyledTextSlice StyledText::Slice(const Range& range) const {

    CheckRange(range);

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

            Range new_range = each_item.Range();

            auto cloned_object = As<textual::InlineObject>(
                each_item.InlineObject()->GetType()->CreateInstance());

            slice_style.AttachInlineObjectToRange(cloned_object, new_range);
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

        auto cloned_object = As<textual::InlineObject>(
            each_item.InlineObject()->GetType()->CreateInstance());

        ranged_style_.AttachInlineObjectToRange(cloned_object, each_item.Range());
    }
}


void StyledText::CheckRange(const Range& range) const {

    ZAF_EXPECT(
        (range.index <= text_.length()) &&
        (range.EndIndex() <= text_.length()));
}

}