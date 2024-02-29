#pragma once

#include <string>
#include <zaf/base/error/check.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/enumerator.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/control/internal/textual_control/inline_object_wrapper.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/text/custom_text_inline_object.h>

namespace zaf::internal {

class StyledText : NonCopyable {
private:
    using RangedFontMap = RangeMap<zaf::Font>;
    using RangedColorPickerMap = RangeMap<zaf::ColorPicker>;
    using InlineObjectMap = RangeMap<InlineObjectWrapper>;

public:
    class RangedFont : NonCopyable {
    public:
        explicit RangedFont(const RangedFontMap::Item& item) : inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const Font& Font() const { return inner_.Value(); }
    private:
        RangedFontMap::Item inner_;
    };

    using RangedFontEnumerator = WrapEnumerator<RangedFontMap, RangedFont>;

    class RangedColorPicker : NonCopyable {
    public:
        explicit RangedColorPicker(const RangedColorPickerMap::Item& item) : inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const ColorPicker& ColorPicker() const { return inner_.Value(); }
    private:
        RangedColorPickerMap::Item inner_;
    };

    using RangedColorPickerEnumerator = WrapEnumerator<RangedColorPickerMap, RangedColorPicker>;

    class RangedInlineObject : NonCopyable {
    public:
        explicit RangedInlineObject(const InlineObjectMap::Item& item) : inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const std::shared_ptr<CustomTextInlineObject>& InlineObject() const { 
            return inner_.Value().Object();
        }
    private:
        InlineObjectMap::Item inner_;
    };

    using InlineObjectEnumerator = WrapEnumerator<InlineObjectMap, RangedInlineObject>;

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
        std::shared_ptr<CustomTextInlineObject> object,
        const Range& range);

    void ClearInlineObjects();

    std::shared_ptr<CustomTextInlineObject> GetInlineObjectAtIndex(std::size_t index) const;
    InlineObjectEnumerator InlineObjects() const;

    StyledText Slice(const Range& range) const;

private:
    void CheckRange(const Range& range) const;

private:
    std::wstring text_;

    Font default_font_{ Font::Default() };
    ColorPicker default_text_color_picker_{ CreateColorPicker(Color::Black()) };

    RangedFontMap ranged_fonts_;
    RangedColorPickerMap ranged_text_color_pickers_;
    InlineObjectMap inline_objects_;
};

}