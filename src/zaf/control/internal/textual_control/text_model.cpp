#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/base/error/check.h>

namespace zaf::internal {

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
    styled_text_.ClearRangedTextColorPicker();

    RaiseChangedEvent(TextModelAttribute::TextColor);
}


void TextModel::SetTextColorPickerInRange(ColorPicker picker, const Range& range) {

    styled_text_.SetTextColorPickerInRange(std::move(picker), range);
    RaiseChangedEvent(TextModelAttribute::TextColor, range, range.length);
}


void TextModel::SetInlineObjectInRange(
    std::shared_ptr<CustomTextInlineObject> object,
    const Range& range) {

    styled_text_.SetInlineObjectInRange(std::move(object), range);
    RaiseChangedEvent(TextModelAttribute::InlineObject, range, range.length);
}

}