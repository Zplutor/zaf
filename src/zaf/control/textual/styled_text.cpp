#include "styled_text.h"
#include <zaf/control/textual/styled_text.h>
#include <zaf/base/as.h>
#include <zaf/xml/xml_error.h>
#include <zaf/xml/xml_reader.h>
#include <zaf/xml/xml_writer.h>

namespace zaf::textual {

StyledText::StyledText() {

}


StyledText::StyledText(std::wstring text) : text_(std::move(text)) {

}


StyledText::StyledText(const StyledTextView& styled_text_view) {
    AssignStyledTextFromView(*this, styled_text_view, true);
}


void StyledText::AssignStyledTextFromView(
    StyledText& styled_text,
    const StyledTextView& view,
    bool assign_objects) {

    //Use SetText() to clear all ranged styles.
    styled_text.SetText(std::wstring{ view.Text() });
    styled_text.default_style_ = view.DefaultStyle();

    for (const auto& each_item : view.RangedFonts()) {
        styled_text.ranged_style_.SetFontInRange(each_item.Value(), each_item.Range());
    }

    for (const auto& each_item : view.RangedTextColors()) {
        styled_text.ranged_style_.SetTextColorInRange(each_item.Value(), each_item.Range());
    }

    for (const auto& each_item : view.RangedTextBackColors()) {
        styled_text.ranged_style_.SetTextBackColorInRange(each_item.Value(), each_item.Range());
    }

    if (assign_objects) {

        for (const auto& each_item : view.InlineObjects()) {

            styled_text.ranged_style_.AttachInlineObjectToRange(
                each_item.Object()->Clone(),
                each_item.Range());
        }
    }
}


void StyledText::SetText(std::wstring text) {

    text_ = std::move(text);
    ranged_style_.Clear();
}


Range StyledText::SetTextInRange(
    std::wstring_view text, 
    const Range& range,
    StyledText* old_styled_text) {

    ZAF_EXPECT(range.index <= text_.length());

    //Preserve the old styled text in the range.
    //Inline objects should be handled carefully, as we want to return the original objects to 
    //old_styled_text directly without cloning them.
    std::vector<std::pair<Range, std::shared_ptr<InlineObject>>> inline_objects;
    if (old_styled_text) {

        StyledTextView view{ *this, range };
        AssignStyledTextFromView(*old_styled_text, view, false);

        for (const auto& each_item : view.InlineObjects()) {
            inline_objects.emplace_back(each_item.Range(), each_item.Object());
        }
    }

    text_.replace(range.index, range.length, text);
    ranged_style_.ReplaceSpan(range, text.length());

    if (old_styled_text) {
        for (auto& each_item : inline_objects) {
            old_styled_text->ranged_style_.AttachInlineObjectToRange(
                std::move(each_item.second), 
                each_item.first);
        }
    }

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

    if (style.TextColor()) {
        ranged_style_.SetTextColorInRange(*style.TextColor(), new_range);
    }

    if (style.TextBackColor()) {
        ranged_style_.SetTextBackColorInRange(*style.TextBackColor(), new_range);
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
        return iterator->Value();
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
    CheckRange(range);
    ranged_style_.SetTextColorInRange(color, range);
}


void StyledText::ClearRangedTextColors() {
    ranged_style_.ClearTextColors();
}


const Color& StyledText::GetTextColorAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto& text_colors = ranged_style_.TextColors();
    auto iterator = text_colors.FindItemAtIndex(index);
    if (iterator != text_colors.end()) {
        return iterator->Value();
    }
    return default_style_.TextColor();
}


const StyledText::RangedColorAccessor& StyledText::RangedTextColors() const {
    return ranged_style_.TextColors();
}


StyledText::RangedColorAccessor& StyledText::RangedTextColors() {
    return ranged_style_.TextColors();
}


void StyledText::SetTextBackColorInRange(const Color& color, const Range& range) {
    CheckRange(range);
    ranged_style_.SetTextBackColorInRange(color, range);
}


void StyledText::ClearRangedTextBackColors() {
    ranged_style_.ClearTextBackColors();
}


const Color& StyledText::GetTextBackColorAtIndex(std::size_t index) const {

    ZAF_EXPECT(index <= text_.length());

    auto& text_back_colors = ranged_style_.TextBackColors();
    auto iterator = text_back_colors.FindItemAtIndex(index);
    if (iterator != text_back_colors.end()) {
        return iterator->Value();
    }
    return default_style_.TextBackColor();
}


const StyledText::RangedColorAccessor& StyledText::RangedTextBackColors() const {
    return ranged_style_.TextBackColors();
}


StyledText::RangedColorAccessor& StyledText::RangedTextBackColors() {
    return ranged_style_.TextBackColors();
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


StyledText StyledText::GetSubText(const Range& range) const {
    StyledTextView view{ *this, range };
    return StyledText{ view };
}


Range StyledText::SetStyledTextInRange(
    StyledText styled_text,
    const Range& range,
    StyledText* old_styled_text) {

    //Text
    auto new_range = SetTextInRange(styled_text.Text(), range, old_styled_text);

    //Default style
    const auto& other_default_style = styled_text.DefaultStyle();
    if (other_default_style.Font() != this->DefaultFont()) {
        ranged_style_.SetFontInRange(other_default_style.Font(), new_range);
    }

    //Default text color 
    if (other_default_style.TextColor() != this->DefaultTextColor()) {
        ranged_style_.SetTextColorInRange(other_default_style.TextColor(), new_range);
    }

    //Default text back color picker
    if (other_default_style.TextBackColor() != this->DefaultTextBackColor()) {
        ranged_style_.SetTextBackColorInRange(other_default_style.TextBackColor(), new_range);
    }

    const auto revise_item_range = [&new_range](const Range& item_range) {
        return ReviseItemRangeForSettingSubText(item_range, new_range);
    };

    //Ranged fonts
    for (auto& each_item : styled_text.RangedFonts()) {
        ranged_style_.SetFontInRange(
            std::move(each_item.Value()), //Move fonts into the current styled text.
            revise_item_range(each_item.Range()));
    }

    //Ranged text color
    for (const auto& each_item : styled_text.RangedTextColors()) {
        ranged_style_.SetTextColorInRange(each_item.Value(), revise_item_range(each_item.Range()));
    }

    //Ranged text back color
    for (const auto& each_item : styled_text.RangedTextBackColors()) {
        ranged_style_.SetTextBackColorInRange(
            each_item.Value(),
            revise_item_range(each_item.Range()));
    }

    //Inline objects
    std::vector<std::pair<Range, std::shared_ptr<InlineObject>>> inline_objects;
    for (const auto& each_item : styled_text.InlineObjects()) {
        inline_objects.emplace_back(each_item.Range(), each_item.Object());
    }
    styled_text.ClearInlineObjects();

    for (auto& each_item : inline_objects) {
        ranged_style_.AttachInlineObjectToRange(
            std::move(each_item.second),
            revise_item_range(each_item.first));
    }

    return new_range;
}


Range StyledText::ReviseItemRangeForSettingSubText(
    const Range& item_range,
    const Range& sub_text_range) {

    Range result = item_range;

    if (result.EndIndex() > sub_text_range.EndIndex()) {
        result.length -= result.EndIndex() - sub_text_range.EndIndex();
    }

    result.index += sub_text_range.index;
    return result;
}


void StyledText::Truncate(std::size_t new_length) {

    ZAF_EXPECT(new_length <= this->Length());
    this->SetTextInRange({}, { new_length, this->Length() - new_length });
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


void StyledText::WriteToXML(XMLWriter& writer) const {

    writer.WriteElementStart(L"StyledText");

    writer.WriteElementStart(L"Text");
    writer.WriteCDATA(this->Text());
    writer.WriteElementEnd();

    default_style_.WriteToXML(writer);
    ranged_style_.WriteToXML(writer);

    writer.WriteElementEnd();
}


void StyledText::ReadFromXML(XMLReader& reader) {

    reader.ReadNotEmptyElementStart(L"StyledText");

    ReadTextFromXML(reader);

    default_style_.ReadFromXML(reader);
    ranged_style_.ReadFromXML(reader);

    reader.ReadElementEnd();
}


void StyledText::ReadTextFromXML(XMLReader& reader) {

    auto [is_empty] = reader.ReadElementStart(L"Text");
    if (is_empty) {
        return;
    }

    switch (reader.GetNodeType()) {
    case XMLNodeType::Text:
    case XMLNodeType::CDATA:
        SetText(std::wstring{ reader.GetValue() });
        reader.Read();
        break;
    case XMLNodeType::ElementEnd:
        break;
    default:
        throw XMLError{ ZAF_SOURCE_LOCATION() };
    }

    reader.ReadElementEnd();
}

}