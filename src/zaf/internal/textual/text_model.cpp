#include <zaf/internal/textual/text_model.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/internal/textual/line_utility.h>

namespace zaf::internal {

TextModel::TextModel() {

}


bool TextModel::IsMultiline() const noexcept {
    return is_multiline_;
}

void TextModel::SetIsMultiline(bool is_multiline) {

    if (is_multiline_ == is_multiline) {
        return;
    }

    is_multiline_ = is_multiline;

    //Remove lines if multiline is set to false.
    if (!is_multiline_) {

        const auto& text = this->Text();
        auto line_break_index = text.find_first_of(L"\r\n");
        if (line_break_index != std::wstring::npos) {
            this->SetTextInRange({}, { line_break_index, text.length() - line_break_index });
        }
    }
}


textual::LineBreak TextModel::LineBreak() const noexcept {
    return line_break_;
}


void TextModel::SetLineBreak(textual::LineBreak line_break) {

    if (line_break_ == line_break) {
        return;
    }

    line_break_ = line_break;
}


const textual::StyledText& TextModel::StyledText() const noexcept {
    return styled_text_;
}

void TextModel::SetStyledText(textual::StyledText styled_text) {

    internal::LineUtility::ReviseLinesInStyledText(styled_text, is_multiline_, line_break_);

    //Replace the old styled text.
    styled_text_ = std::move(styled_text);

    default_text_color_picker_ = nullptr;
    ranged_text_color_pickers_.Clear();
    default_text_back_color_picker_ = nullptr;
    ranged_text_back_color_pickers_.Clear();

    //Get all inline objects from new styled text.
    std::vector<std::shared_ptr<textual::InlineObject>> new_inline_objects;
    for (const auto& each_item : styled_text_.InlineObjects()) {
        new_inline_objects.push_back(each_item.Object());
    }

    //Raise events.
    RaiseInlineObjectAttachedEvent(std::move(new_inline_objects));
    RaiseChangedEvent(TextModelAttribute::All);
}


Range TextModel::SetStyledTextInRange(const textual::StyledText& styled_text, const Range& range) {

    auto single_line_range = TryReplaceStyledTextSliceAsSingleLine(range, styled_text);
    if (single_line_range) {
        return *single_line_range;
    }

    InnerReplaceStyledTextSlice(range, styled_text);
    return Range{ range.index, styled_text.Length() };
}


std::optional<Range> TextModel::TryReplaceStyledTextSliceAsSingleLine(
    const Range& replaced_range,
    const textual::StyledText& slice) {

    if (is_multiline_) {
        return std::nullopt;
    }

    const auto& slice_text = slice.Text();
    auto line_break_index = slice_text.find_first_of(L"\r\n");
    if (line_break_index == slice_text.npos) {
        return std::nullopt;
    }

    auto single_line_slice = slice.GetSubText({ 0, line_break_index });

    InnerReplaceStyledTextSlice(replaced_range, single_line_slice);
    return Range{ replaced_range.index, single_line_slice.Length() };
}


void TextModel::InnerReplaceStyledTextSlice(
    const Range& replaced_range,
    const textual::StyledText& slice) {

    auto sub_text_range = styled_text_.SetStyledTextInRange(slice, replaced_range);
    ranged_text_color_pickers_.RemoveRange(sub_text_range);
    ranged_text_back_color_pickers_.RemoveRange(sub_text_range);

    //Get inline objects from the slice range to raise attach event.
    std::vector<std::shared_ptr<textual::InlineObject>> new_inline_objects;
    styled_text_.InlineObjects().VisitItemsInRange(
        sub_text_range,
        [&new_inline_objects](const auto& item) {
        new_inline_objects.push_back(item.Object());
    });

    RaiseInlineObjectAttachedEvent(std::move(new_inline_objects));
    RaiseChangedEvent(TextModelAttribute::All);
}


const std::wstring& TextModel::Text() const noexcept {
    return styled_text_.Text();
}


void TextModel::SetText(std::wstring text) {

    internal::LineUtility::ReviseLinesInText(text, is_multiline_, line_break_);

    styled_text_.SetText(std::move(text));
    ranged_text_color_pickers_.Clear();
    ranged_text_back_color_pickers_.Clear();

    RaiseChangedEvent(TextModelAttribute::All);
}


void TextModel::SetTextInRange(std::wstring_view text, Range range) {

    ZAF_EXPECT(
        (range.index <= styled_text_.Text().length()) &&
        (range.EndIndex() <= styled_text_.Text().length()));

    //Remove lines if multiline is not supported.
    if (!is_multiline_) {

        auto line_break_index = text.find_first_of(L"\r\n");
        if (line_break_index != std::wstring_view::npos) {
            text = text.substr(0, line_break_index);
        }
    }

    styled_text_.SetTextInRange(text, range);
    ranged_text_color_pickers_.ReplaceSpan(range, text.length());
    ranged_text_back_color_pickers_.ReplaceSpan(range, text.length());

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


void TextModel::SetTextColor(const Color& color) {

    default_text_color_picker_ = nullptr;
    ranged_text_color_pickers_.Clear();

    InnerSetTextColor(color);
}


void TextModel::SetTextColorPicker(ColorPicker picker, const Control& owner) {

    auto text_color = picker(owner);

    default_text_color_picker_ = std::move(picker);
    ranged_text_color_pickers_.Clear();
    
    InnerSetTextColor(text_color);
}


void TextModel::InnerSetTextColor(const Color& color) {

    styled_text_.SetDefaultTextColor(color);
    styled_text_.ClearRangedTextColors();
    RaiseChangedEvent(TextModelAttribute::TextColor);
}


void TextModel::SetTextColorInRange(const Color& color, const Range& range) {

    ranged_text_color_pickers_.RemoveRange(range);
    InnerSetTextColorInRange(color, range);
}


void TextModel::SetTextColorPickerInRange(
    ColorPicker picker,
    const Range& range,
    const Control& owner) {

    auto text_color = picker(owner);
    ranged_text_color_pickers_.AddRange(range, std::move(picker));

    InnerSetTextColorInRange(text_color, range);
}


void TextModel::InnerSetTextColorInRange(const Color& color, const Range& range) {
    styled_text_.SetTextColorInRange(color, range);
    RaiseChangedEvent(TextModelAttribute::TextColor, range, range.length);
}


void TextModel::SetTextBackColor(const Color& color) {

    default_text_back_color_picker_ = nullptr;
    ranged_text_back_color_pickers_.Clear();

    InnerSetTextBackColor(color);
}


void TextModel::SetTextBackColorPicker(ColorPicker picker, const Control& owner) {

    auto back_color = picker(owner);

    default_text_back_color_picker_ = std::move(picker);
    ranged_text_back_color_pickers_.Clear();

    InnerSetTextBackColor(back_color);
}


void TextModel::InnerSetTextBackColor(const Color& color) {

    styled_text_.SetDefaultTextBackColor(color);
    styled_text_.ClearRangedTextBackColors();
    RaiseChangedEvent(TextModelAttribute::TextBackColor);
}


void TextModel::SetTextBackColorInRange(const Color& color, const Range& range) {

    ranged_text_back_color_pickers_.RemoveRange(range);
    InnerSetTextBackColorInRange(color, range);
}


void TextModel::SetTextBackColorPickerInRange(
    ColorPicker picker,
    const Range& range,
    const Control& owner) {

    auto back_color = picker(owner);
    ranged_text_back_color_pickers_.AddRange(range, std::move(picker));
    InnerSetTextBackColorInRange(back_color, range);
}


void TextModel::InnerSetTextBackColorInRange(const Color& color, const Range& range) {

    styled_text_.SetTextBackColorInRange(color, range);
    RaiseChangedEvent(TextModelAttribute::TextBackColor);
}


void TextModel::UpdateColors(const Control& owner) {

    //Update text colors.
    if (default_text_color_picker_) {
        auto default_text_color = default_text_color_picker_(owner);
        styled_text_.SetDefaultTextColor(default_text_color);
    }

    for (const auto& each_item : ranged_text_color_pickers_) {
        auto text_color = each_item.Value()(owner);
        styled_text_.SetTextColorInRange(text_color, each_item.Range());
    }

    //Update text back colors.
    if (default_text_back_color_picker_) {
        auto default_text_back_color = default_text_back_color_picker_(owner);
        styled_text_.SetDefaultTextBackColor(default_text_back_color);
    }

    for (const auto& each_item : ranged_text_back_color_pickers_) {
        auto text_back_color = each_item.Value()(owner);
        styled_text_.SetTextBackColorInRange(text_back_color, each_item.Range());
    }
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


void TextModel::RaiseInlineObjectAttachedEvent(
    std::vector<std::shared_ptr<textual::InlineObject>> objects) {

    if (objects.empty()) {
        return;
    }

    inline_object_attached_event_.AsObserver().OnNext(InlineObjectAttachedInfo{
        std::move(objects),
    });
}


void TextModel::RaiseChangedEvent(TextModelAttribute attributes) {
    changed_event_.AsObserver().OnNext(TextModelChangedInfo{ attributes });
}


void TextModel::RaiseChangedEvent(
    TextModelAttribute attributes,
    const Range& changed_range,
    std::size_t new_length) {

    changed_event_.AsObserver().OnNext(TextModelChangedInfo{
        attributes,
        changed_range,
        new_length
    });
}

}