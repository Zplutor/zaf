#include <zaf/internal/textual/text_model.h>
#include <zaf/base/error/contract.h>

namespace zaf::internal {

TextModel::TextModel() {

}


void TextModel::SetStyledText(textual::StyledText styled_text) {

    //Replace the old styled text.
    styled_text_ = std::move(styled_text);

    //Get all inline objects from new styled text.
    std::vector<std::shared_ptr<textual::InlineObject>> new_inline_objects;
    for (const auto& each_item : styled_text_.InlineObjects()) {
        new_inline_objects.push_back(each_item.Object());
    }

    //Raise events.
    RaiseInlineObjectAttachedEvent(std::move(new_inline_objects));
    RaiseChangedEvent(TextModelAttribute::All);
}


void TextModel::SetText(std::wstring text) {

    auto old_length = styled_text_.Text().length();
    auto new_length = text.length();

    styled_text_.SetText(std::move(text));

    RaiseChangedEvent(TextModelAttribute::All);
}


void TextModel::SetTextInRange(std::wstring_view text, const Range& range) {

    ZAF_EXPECT(
        (range.index <= styled_text_.Text().length()) &&
        (range.EndIndex() <= styled_text_.Text().length()));

    styled_text_.SetTextInRange(text, range);

    RaiseChangedEvent(TextModelAttribute::All, range, text.length());
}


void TextModel::SetFont(Font font) {

    styled_text_.SetDefaultFont(std::move(font));
    styled_text_.ClearRangedFonts();

    RaiseChangedEvent(TextModelAttribute::Font);
}


void TextModel::SetFontInRange(Font font, const Range& range) {

    styled_text_.SetFontInRange(std::move(font), range);
    RaiseChangedEvent(TextModelAttribute::Font, range, range.length);
}


void TextModel::SetTextColorPicker(ColorPicker picker) {

    styled_text_.SetDefaultTextColorPicker(std::move(picker));
    styled_text_.ClearRangedTextColorPickers();
    RaiseChangedEvent(TextModelAttribute::TextColor);
}


void TextModel::SetTextColorPickerInRange(ColorPicker picker, const Range& range) {

    styled_text_.SetTextColorPickerInRange(std::move(picker), range);
    RaiseChangedEvent(TextModelAttribute::TextColor, range, range.length);
}


void TextModel::SetTextBackColorPicker(ColorPicker picker) {

    styled_text_.SetDefaultTextBackColorPicker(std::move(picker));
    styled_text_.ClearRangedTextBackColorPickers();
    RaiseChangedEvent(TextModelAttribute::TextBackColor);
}


void TextModel::SetTextBackColorPickerInRange(ColorPicker picker, const Range& range) {

    styled_text_.SetTextBackColorPickerInRange(std::move(picker), range);
    RaiseChangedEvent(TextModelAttribute::TextBackColor);
}


void TextModel::AttachInlineObjectToRange(
    std::shared_ptr<textual::InlineObject> object,
    const Range& range) {

    if (range.IsEmpty()) {
        return;
    }

    styled_text_.AttachInlineObjectToRange(object, range);

    RaiseInlineObjectAttachedEvent({ std::move(object) });
    RaiseChangedEvent(TextModelAttribute::InlineObject, range, range.length);
}


void TextModel::ReplaceStyledTextSlice(
    const Range& replaced_range, 
    const textual::StyledTextSlice& slice) {

    std::vector<std::shared_ptr<textual::InlineObject>> new_inline_objects;
    for (const auto& each_item : slice.RangedStyle().InlineObjects()) {
        new_inline_objects.push_back(each_item.Object());
    }

    styled_text_.ReplaceSlice(replaced_range, slice);
    RaiseChangedEvent(TextModelAttribute::All);
}

}