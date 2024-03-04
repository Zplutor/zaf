#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/enumerator.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/control/internal/textual_control/inline_object_wrapper.h>
#include <zaf/graphic/font/font.h>

namespace zaf::internal {

class RangedStyle {
public:
    using RangedFontMap = RangeMap<zaf::Font>;
    using RangedColorPickerMap = RangeMap<zaf::ColorPicker>;
    using InlineObjectMap = RangeMap<InlineObjectWrapper>;

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
    RangedFontEnumerator RangedFonts() const {
        return RangedFontEnumerator{ ranged_fonts_ };
    }

    const Font* GetFontAtIndex(std::size_t index) const {
        return ranged_fonts_.GetValueAtIndex(index);
    }

    void SetFontInRange(Font font, const Range& range) {
        ranged_fonts_.AddRange(range, std::move(font));
    }

    RangedColorPickerEnumerator RangedTextColorPicker() const {
        return RangedColorPickerEnumerator{ ranged_text_color_pickers_ };
    }

    const ColorPicker* GetTextColorPickerAtIndex(std::size_t index) const {
        return ranged_text_color_pickers_.GetValueAtIndex(index);
    }

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        ranged_text_color_pickers_.AddRange(range, std::move(color_picker));
    }

    InlineObjectEnumerator InlineObjects() const {
        return InlineObjectEnumerator{ inline_objects_ };
    }

    std::shared_ptr<CustomTextInlineObject> GetInlineObjectAtIndex(std::size_t index) const {
        if (auto wrapper = inline_objects_.GetValueAtIndex(index)) {
            return wrapper->Object();
        }
        return nullptr;
    }

    void AttachInlineObjectToRange(
        std::shared_ptr<CustomTextInlineObject> object,
        const Range& range) {

        ZAF_EXPECT(object);
        inline_objects_.RemoveRangesIntersectWith(range);
        inline_objects_.AddRange(range, InlineObjectWrapper{ std::move(object) });
    }

private:
    RangedFontMap ranged_fonts_;
    RangedColorPickerMap ranged_text_color_pickers_;
    InlineObjectMap inline_objects_;
};

}