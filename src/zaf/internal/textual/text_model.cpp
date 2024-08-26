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

        bool has_revised = internal::ReviseLinesInStyledText(
            styled_text_,
            is_multiline_,
            line_break_,
            [this](const Range& range, std::size_t new_length) {
                ranged_text_color_pickers_.ReplaceSpan(range, new_length);
                ranged_text_back_color_pickers_.ReplaceSpan(range, new_length);
            });

        if (has_revised) {
            RaiseChangedEvent(TextModelAttribute::All);
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

    //Revise line breaks in the current styled text.
    if (is_multiline_) {

        bool has_revised = internal::ReviseLinesInStyledText(
            styled_text_,
            is_multiline_,
            line_break_,
            [this](const Range& range, std::size_t new_length) {
                ranged_text_color_pickers_.ReplaceSpan(range, new_length);
                ranged_text_back_color_pickers_.ReplaceSpan(range, new_length);
            });

        if (has_revised) {
            RaiseChangedEvent(TextModelAttribute::All);
        }
    }
}


const textual::StyledText& TextModel::StyledText() const noexcept {
    return styled_text_;
}

void TextModel::SetStyledText(textual::StyledText styled_text) {

    internal::ReviseLinesInStyledText(styled_text, is_multiline_, line_break_, nullptr);

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

    auto revised_text = internal::ReviseLinesInStyledTextView(
        styled_text, 
        is_multiline_,
        line_break_);

    const textual::StyledText* styled_text_to_set{};
    if (std::holds_alternative<std::reference_wrapper<const textual::StyledText>>(revised_text)) {
        styled_text_to_set = 
            &std::get<std::reference_wrapper<const textual::StyledText>>(revised_text).get();
    }
    else {
        styled_text_to_set = &std::get<textual::StyledText>(revised_text);
    }

    InnerSetStyledTextInRange(*styled_text_to_set, range);
    return Range{ range.index, styled_text_to_set->Length() };
}


void TextModel::InnerSetStyledTextInRange(
    const textual::StyledText& slice,
    const Range& replaced_range) {

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

    internal::ReviseLinesInText(text, is_multiline_, line_break_);

    styled_text_.SetText(std::move(text));
    ranged_text_color_pickers_.Clear();
    ranged_text_back_color_pickers_.Clear();

    RaiseChangedEvent(TextModelAttribute::All);
}


void TextModel::SetTextInRange(std::wstring_view text, Range range) {

    ZAF_EXPECT(
        (range.index <= styled_text_.Text().length()) &&
        (range.EndIndex() <= styled_text_.Text().length()));

    auto revised_text = internal::ReviseLinesInTextView(
        text,
        is_multiline_,
        line_break_);

    auto text_to_set = std::visit([](const auto& text) -> std::wstring_view {
        return text;
    },
    revised_text);

    styled_text_.SetTextInRange(text_to_set, range);
    ranged_text_color_pickers_.ReplaceSpan(range, text_to_set.length());
    ranged_text_back_color_pickers_.ReplaceSpan(range, text_to_set.length());

    RaiseChangedEvent(TextModelAttribute::All, range, text_to_set.length());
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