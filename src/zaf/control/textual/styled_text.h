#pragma once

/**
@file
    Defines the zaf::textual::StyledText class.
*/

#include <string>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/styled_text_slice.h>
#include <zaf/control/textual/text_style.h>
#include <zaf/graphic/font/font.h>

namespace zaf::textual {

/**
Represents a text with various styles, such as the font, text color, and inline objects.

@details
    A StyledText has two kinds of styles:
    - Ranged styles, which apply to a specified sub-range of the text.
    - Default styles, which apply to all text without ranged styles.

    Copying a StyledText is a high-cost operation, so copying is forbidden. Users can use the
    Clone() method to copy the StyledText if needed.
*/
class StyledText : NonCopyable {
public:
    /**
    A class used to access the ranged fonts in a StyledText.
    */
    using RangedFontAccessor = RangedTextStyle::FontAccessor;

    /**
    A class used to access the ranged color pickers in a StyledText.
    */
    using RangedColorPickerAccessor = RangedTextStyle::ColorPickerAccessor;

public:
    /**
    Constructs an empty StyledText.
    */
    StyledText() noexcept;

    /**
    Constructs a StyledText with the specified text.

    @param text
        The text that the StyledText will own.
    */
    explicit StyledText(std::wstring text) noexcept;

    /**
    Gets the text of the StyledText.

    @return
        The text of the StyledText.
    */
    const std::wstring& Text() const noexcept {
        return text_;
    }

    /**
    Sets the specified text to the StyledText.

    @param text
        The text to be set.
    */
    void SetText(std::wstring text);

    /**
    Sets the specified text to the specified range within the existing text.

    @param text
        The text to set.

    @param range
        The range within the existing text where the new text will be set.

    @pre 
        The range is entirely within the existing text.

    @return 
        The range of the new text within the whole text after setting.

    @remark
        The style of the new text will be set to default.
    */
    Range SetTextInRange(std::wstring_view text, const Range& range);

    /**
    Appends the specified text to the end of the existing text.

    @param text
        The text to append.

    @return 
        The range of the new text within the whole text after appending.

    @remark
        The style of the new text will be set to default.
    */
    Range AppendText(std::wstring_view text);

    /**
    Appends the specified text to the end of the existing text, and applies the specified text
    style to the new text.

    @param text
        The text to append.

    @param style
        The text style to apply to the new text.

    @return
        The range of the new text within the whole text after appending.
    */
    Range AppendText(std::wstring_view text, const TextStyle& style);

    void ClearText();

    const Font& DefaultFont() const {
        return default_font_;
    }

    void SetDefaultFont(Font font) {
        default_font_ = std::move(font);
    }

    void SetFontInRange(Font font, const Range& range);
    void ClearRangedFonts();
    const Font& GetFontAtIndex(std::size_t index) const;
    const RangedFontAccessor& RangedFonts() const;
    RangedFontAccessor& RangedFonts();

    const ColorPicker& DefaultTextColorPicker() const {
        return default_text_color_picker_;
    }

    void SetDefaultTextColor(const Color& color);
    void SetDefaultTextColorPicker(ColorPicker color_picker);
    void SetTextColorInRange(const Color& color, const Range& range);
    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range);
    void ClearRangedTextColorPickers();
    const ColorPicker& GetTextColorPickerAtIndex(std::size_t index) const;
    const RangedColorPickerAccessor& RangedTextColorPickers() const;
    RangedColorPickerAccessor& RangedTextColorPickers();

    const ColorPicker& DefaultTextBackColorPicker() const {
        return default_text_back_color_picker_;
    }

    void SetDefaultTextBackColor(const Color& color);
    void SetDefaultTextBackColorPicker(ColorPicker color_picker);
    void SetTextBackColorInRange(const Color& color, const Range& range);
    void SetTextBackColorPickerInRange(ColorPicker color_picker, const Range& range);
    void ClearRangedTextBackColorPickers();
    const ColorPicker& GetTextBackColorPickerAtIndex(std::size_t index) const;
    const RangedColorPickerAccessor& RangedTextBackColorPickers() const;
    RangedColorPickerAccessor& RangedTextBackColorPickers();

    void AttachInlineObjectToRange(
        std::shared_ptr<InlineObject> object,
        const Range& range);

    void ClearInlineObjects();

    std::shared_ptr<InlineObject> GetInlineObjectAtIndex(std::size_t index) const;
    const InlineObjectAccessor& InlineObjects() const;

    StyledTextSlice Slice(const Range& range) const;
    void ReplaceSlice(const Range& slice_range, const StyledTextSlice& new_slice);

    /**
    Gets a sub-portion of the styled text.

    @param range
        The range of the sub-portion within the styled text. The end index may exceed the length of
        the styled text; in such a case, the sub-portion contains the text up to the end.

    @pre
        The start index of range does not exceed the length of the style text.

    @return
        The sub-portion of the styled text.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    StyledText GetSubText(const Range& range) const;

    StyledText Clone() const;

private:
    void CheckRange(const Range& range) const;

private:
    std::wstring text_;

    Font default_font_{ Font::Default() };
    ColorPicker default_text_color_picker_{ CreateColorPicker(Color::Black()) };
    ColorPicker default_text_back_color_picker_{ CreateColorPicker(Color::Transparent()) };

    RangedTextStyle ranged_style_;
};

}