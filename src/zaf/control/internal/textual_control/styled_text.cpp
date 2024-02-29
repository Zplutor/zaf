#include <zaf/control/internal/textual_control/styled_text.h>

namespace zaf::internal {

StyledText::StyledText() {

}


StyledText::StyledText(std::wstring text) : text_(std::move(text)) {

}


void StyledText::SetText(std::wstring text) {

    text_ = std::move(text);

    ranged_fonts_.Clear();
    ranged_text_color_pickers_.Clear();
    inline_objects_.Clear();
}


void StyledText::SetTextInRange(std::wstring_view text, const Range& range) {

    CheckRange(range);

    text_.replace(range.index, range.length, text);

    ranged_fonts_.ReplaceSpan(range, text.length());
    ranged_text_color_pickers_.ReplaceSpan(range, text.length());

    inline_objects_.RemoveRangesIntersectWith(range);
    inline_objects_.ReplaceSpan(range, text.length());
}


void StyledText::ClearText() {
    SetText({});
}


void StyledText::SetFontInRange(Font font, const Range& range) {

    CheckRange(range);
    ranged_fonts_.AddRange(range, std::move(font));
}


void StyledText::ClearRangedFonts() {
    ranged_fonts_.Clear();
}


const Font& StyledText::GetFontAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto font = ranged_fonts_.GetValueAtIndex(index);
    if (font) {
        return *font;
    }
    return default_font_;
}


StyledText::RangedFontEnumerator StyledText::RangedFonts() const {
    return RangedFontEnumerator{ ranged_fonts_ };
}


void StyledText::SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {

    CheckRange(range);
    ranged_text_color_pickers_.AddRange(range, std::move(color_picker));
}


void StyledText::ClearRangedTextColorPicker() {
    ranged_text_color_pickers_.Clear();
}


const ColorPicker& StyledText::GetTextColorPickerAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto picker = ranged_text_color_pickers_.GetValueAtIndex(index);
    if (picker) {
        return *picker;
    }
    return default_text_color_picker_;
}


StyledText::RangedColorPickerEnumerator StyledText::RangedTextColorPicker() const {
    return RangedColorPickerEnumerator{ ranged_text_color_pickers_ };
}


void StyledText::AttachInlineObjectToRange(
    std::shared_ptr<CustomTextInlineObject> object,
    const Range& range) {

    CheckRange(range);
    ZAF_EXPECT(object);

    inline_objects_.RemoveRangesIntersectWith(range);
    inline_objects_.AddRange(range, InlineObjectWrapper{ std::move(object) });
}


void StyledText::ClearInlineObjects() {
    inline_objects_.Clear();
}


std::shared_ptr<CustomTextInlineObject> StyledText::GetInlineObjectAtIndex(
    std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto object = inline_objects_.GetValueAtIndex(index);
    if (object) {
        return object->Object();
    }

    return nullptr;
}


StyledText::InlineObjectEnumerator StyledText::InlineObjects() const {
    return InlineObjectEnumerator{ inline_objects_ };
}


StyledText StyledText::Slice(const Range& range) const {

    CheckRange(range);

    StyledText result;
    result.SetDefaultFont(DefaultFont());
    result.SetDefaultTextColorPicker(DefaultTextColorPicker());

    if (range.length == 0) {
        return result;
    }

    //Text
    result.SetText(text_.substr(range.index, range.length));

    //Ranged fonts
    for (const auto& each_item : ranged_fonts_) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        auto intersection_range = Range::MakeIntersection(each_item.Range(), range);
        intersection_range.index -= range.index;
        result.SetFontInRange(each_item.Value(), intersection_range);
    }

    //Ranged text color pickers
    for (const auto& each_item : ranged_text_color_pickers_) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        auto intersection_range = Range::MakeIntersection(each_item.Range(), range);
        intersection_range.index -= range.index;
        result.SetTextColorPickerInRange(each_item.Value(), intersection_range);
    }

    //Inline objects.
    for (const auto& each_item : inline_objects_) {

        if (each_item.Range().index >= range.EndIndex()) {
            continue;
        }

        if (each_item.Range().EndIndex() <= range.index) {
            break;
        }

        if (range.Contains(each_item.Range())) {

            Range new_range = each_item.Range();
            new_range.index -= range.index;
            result.AttachInlineObjectToRange(each_item.Value().Object(), new_range);
        }
    }

    return result;
}


void StyledText::CheckRange(const Range& range) const {

    ZAF_EXPECT(
        (range.index <= text_.length()) &&
        (range.EndIndex() <= text_.length()));
}

}