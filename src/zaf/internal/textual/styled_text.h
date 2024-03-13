#pragma once

#include <string>
#include <zaf/base/error/check.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/enumerator.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/internal/textual/styled_text_slice.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/graphic/font/font.h>

namespace zaf::internal {

class StyledText : NonCopyable {
public:
    using RangedFontEnumerator = RangedTextStyle::FontEnumerator;
    using RangedColorPickerEnumerator = RangedTextStyle::ColorPickerEnumerator;
    using InlineObjectEnumerator = RangedTextStyle::InlineObjectEnumerator;

public:
    StyledText();
    explicit StyledText(std::wstring text);

    const std::wstring& Text() const {
        return text_;
    }

    void SetText(std::wstring text);
    void SetTextInRange(std::wstring_view text, const Range& range);
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

    RangedFontEnumerator RangedFonts() const;

    const ColorPicker& DefaultTextColorPicker() const {
        return default_text_color_picker_;
    }

    void SetDefaultTextColorPicker(ColorPicker color_picker) {
        ZAF_EXPECT(color_picker);
        default_text_color_picker_ = std::move(color_picker);
    }

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range);
    void ClearRangedTextColorPicker();

    const ColorPicker& GetTextColorPickerAtIndex(std::size_t index) const;

    RangedColorPickerEnumerator RangedTextColorPicker() const;

    void AttachInlineObjectToRange(
        std::shared_ptr<textual::InlineObject> object,
        const Range& range);

    void ClearInlineObjects();

    std::shared_ptr<textual::InlineObject> GetInlineObjectAtIndex(std::size_t index) const;
    InlineObjectEnumerator InlineObjects() const;

    Observable<InlineObjectChangedInfo> InlineObjectChangedEvent() const {
        return ranged_style_.InlineObjectChangedEvent();
    }

    StyledTextSlice Slice(const Range& range) const;
    void ReplaceSlice(const Range& slice_range, const StyledTextSlice& new_slice);

private:
    void CheckRange(const Range& range) const;

private:
    std::wstring text_;

    Font default_font_{ Font::Default() };
    ColorPicker default_text_color_picker_{ CreateColorPicker(Color::Black()) };

    RangedTextStyle ranged_style_;
};

}