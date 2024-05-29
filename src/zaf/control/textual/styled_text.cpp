#include <zaf/control/textual/styled_text.h>
#include <zaf/base/as.h>

namespace zaf::textual {

StyledText::StyledText() {

}


StyledText::StyledText(std::wstring text) : text_(std::move(text)) {

}


StyledText::StyledText(StyledTextSlice slice) {
    this->SetSlice(std::move(slice));
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


StyledTextSlice StyledText::Slice(const Range& range) const {

    ZAF_EXPECT(range.index <= text_.length());

    //Text
    auto slice_text = text_.substr(range.index, range.length);

    RangedTextStyle ranged_style;

    //Ranged fonts
    ranged_style_.Fonts().VisitItemsInRange(range, [&ranged_style](const auto& item) {
        ranged_style.SetFontInRange(item.Value(), item.Range());
    });

    //Ranged text color
    ranged_style_.TextColors().VisitItemsInRange(range, [&ranged_style](const auto& item) {
        ranged_style.SetTextColorInRange(item.Value(), item.Range());
    });

    //Ranged background color
    ranged_style_.TextBackColors().VisitItemsInRange(
        range, 
        [&ranged_style](const auto& item) {
            ranged_style.SetTextBackColorInRange(item.Value(), item.Range());
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


void StyledText::SetSlice(StyledTextSlice slice) {

    auto slice_index = slice.Index();
    auto slice_length = slice.Length();
    const auto revise_item_range = [slice_index, slice_length](const Range& item_range) {
        return ReviseItemRange(slice_index, slice_length, 0, item_range);
    };

    this->text_ = std::move(slice.text_);
    this->default_style_ = std::move(slice.default_style_);

    this->ranged_style_.Clear();

    //Ranged fonts
    for (auto& each_item : slice.ranged_style_.Fonts()) {
        ranged_style_.SetFontInRange(
            std::move(each_item.Value()),
            revise_item_range(each_item.Range()));
    }

    //Ranged text color picker
    for (auto& each_item : slice.ranged_style_.TextColors()) {
        ranged_style_.SetTextColorInRange(
            std::move(each_item.Value()),
            revise_item_range(each_item.Range()));
    }

    //Ranged text back color picker
    for (auto& each_item : slice.ranged_style_.TextBackColors()) {
        ranged_style_.SetTextBackColorInRange(
            std::move(each_item.Value()),
            revise_item_range(each_item.Range()));
    }

    //Inline objects
    for (auto& each_item : slice.ranged_style_.InlineObjects()) {
        ranged_style_.AttachInlineObjectToRange(
            std::move(each_item.Object()),
            revise_item_range(each_item.Range()));
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

    //Default text color 
    ranged_style_.SetTextColorInRange(slice_default_style.TextColor(), new_range);

    //Default text back color picker
    ranged_style_.SetTextBackColorInRange(slice_default_style.TextBackColor(), new_range);

    const auto revise_item_range = [&slice, &new_range](const Range& item_range) {
        return ReviseItemRange(slice.Index(), slice.Text().length(), new_range.index, item_range);
    };

    //Ranged fonts
    for (const auto& each_item : slice.RangedStyle().Fonts()) {
        ranged_style_.SetFontInRange(each_item.Value(), revise_item_range(each_item.Range()));
    }
    
    //Ranged text color picker
    for (const auto& each_item : slice.RangedStyle().TextColors()) {
        ranged_style_.SetTextColorInRange(each_item.Value(), revise_item_range(each_item.Range()));
    }

    //Ranged text back color picker
    for (const auto& each_item : slice.RangedStyle().TextBackColors()) {
        ranged_style_.SetTextBackColorInRange(
            each_item.Value(),
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
            result.SetFontInRange(item.Value(), revise_item_range(item.Range()));
        });

    //Ranged text color pickers
    ranged_style_.TextColors().VisitItemsInRange(
        range, 
        [&revise_item_range, &result](const auto& item) {
            result.SetTextColorInRange(item.Value(), revise_item_range(item.Range()));
        });

    //Ranged background color pickers
    ranged_style_.TextBackColors().VisitItemsInRange(
        range,
        [&revise_item_range, &result](const auto& item) {
            result.SetTextBackColorInRange(item.Value(), revise_item_range(item.Range()));
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


Range StyledText::ReviseItemRange(
    std::size_t slice_index,
    std::size_t slice_length,
    std::size_t new_index,
    const Range& item_range) {

    Range result;

    if (item_range.index < slice_index) {
        result.index = 0;
        result.length = item_range.length - (slice_index - item_range.index);
    }
    else {
        result.index = item_range.index - slice_index;
        result.length = item_range.length;
    }

    if (result.EndIndex() > slice_length) {
        result.length -= result.EndIndex() - slice_length;
    }

    result.index += new_index;
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