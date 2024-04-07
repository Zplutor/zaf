#pragma once

#include <string>
#include <zaf/base/error/contract.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/styled_text_slice.h>
#include <zaf/control/textual/text_style.h>
#include <zaf/graphic/font/font.h>

namespace zaf::textual {

class StyledText : NonCopyable {
public:
    using RangedFontAccessor = RangedTextStyle::FontAccessor;
    using ConstRangedFontAccessor = RangedTextStyle::ConstFontAccessor;

    using RangedColorPickerAccessor = RangedTextStyle::ColorPickerAccessor;
    using ConstRangedColorPickerAccessor = RangedTextStyle::ConstColorPickerAccessor;

public:
    StyledText();
    explicit StyledText(std::wstring text);

    const std::wstring& Text() const {
        return text_;
    }

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
    ConstRangedFontAccessor RangedFonts() const;
    RangedFontAccessor RangedFonts();

    const ColorPicker& DefaultTextColorPicker() const {
        return default_text_color_picker_;
    }

    void SetDefaultTextColor(const Color& color);
    void SetDefaultTextColorPicker(ColorPicker color_picker);
    void SetTextColorInRange(const Color& color, const Range& range);
    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range);
    void ClearRangedTextColorPickers();
    const ColorPicker& GetTextColorPickerAtIndex(std::size_t index) const;
    ConstRangedColorPickerAccessor RangedTextColorPicker() const;
    RangedColorPickerAccessor RangedTextColorPicker();

    const ColorPicker& DefaultTextBackColorPicker() const {
        return default_text_color_picker_;
    }

    void SetDefaultTextBackColor(const Color& color);
    void SetDefaultTextBackColorPicker(ColorPicker color_picker);
    void SetTextBackColorPickerInRange(ColorPicker color_picker, const Range& range);
    void ClearRangedTextBackColorPickers();
    const ColorPicker& GetTextBackColorPickerAtIndex(std::size_t index) const;
    ConstRangedColorPickerAccessor RangedTextBackColorPickers() const;
    RangedColorPickerAccessor RangedTextBackColorPickers();

    void AttachInlineObjectToRange(
        std::shared_ptr<InlineObject> object,
        const Range& range);

    void ClearInlineObjects();

    std::shared_ptr<InlineObject> GetInlineObjectAtIndex(std::size_t index) const;
    InlineObjectAccessor InlineObjects() const;

    StyledTextSlice Slice(const Range& range) const;
    void ReplaceSlice(const Range& slice_range, const StyledTextSlice& new_slice);

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