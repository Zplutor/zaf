#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/enumerator.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/graphic/font/font.h>
#include <zaf/internal/textual/ranged_inline_object_collection.h>

namespace zaf::internal {

class RangedTextStyle {
public:
    using FontMap = RangeMap<zaf::Font>;
    using ColorPickerMap = RangeMap<zaf::ColorPicker>;

    class FontItem : NonCopyable {
    public:
        explicit FontItem(const FontMap::Item& item) : inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const Font& Font() const { return inner_.Value(); }
    private:
        FontMap::Item inner_;
    };

    using FontEnumerator = WrapEnumerator<FontMap, FontItem>;

    class ColorPickerItem : NonCopyable {
    public:
        explicit ColorPickerItem(const ColorPickerMap::Item& item) : inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const ColorPicker& ColorPicker() const { return inner_.Value(); }
    private:
        ColorPickerMap::Item inner_;
    };

    using ColorPickerEnumerator = WrapEnumerator<ColorPickerMap, ColorPickerItem>;

    class InlineObjectItem : NonCopyable {
    public:
        explicit InlineObjectItem(const RangedInlineObjectCollection::ItemList::value_type& item) : 
            inner_(item) { }
        const Range& Range() const { return inner_.range; }
        const std::shared_ptr<textual::InlineObject>& InlineObject() const {
            return inner_.object_wrapper.Object();
        }
    private:
        const RangedInlineObjectCollection::ItemList::value_type& inner_;
    };

    using InlineObjectEnumerator = WrapEnumerator<
        RangedInlineObjectCollection::ItemList,
        InlineObjectItem
    >;

public:
    FontEnumerator Fonts() const {
        return FontEnumerator{ fonts_ };
    }

    const Font* GetFontAtIndex(std::size_t index) const {
        return fonts_.GetValueAtIndex(index);
    }

    void SetFontInRange(Font font, const Range& range) {
        fonts_.AddRange(range, std::move(font));
    }

    ColorPickerEnumerator TextColorPickers() const {
        return ColorPickerEnumerator{ color_pickers_ };
    }

    const ColorPicker* GetTextColorPickerAtIndex(std::size_t index) const {
        return color_pickers_.GetValueAtIndex(index);
    }

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        color_pickers_.AddRange(range, std::move(color_picker));
    }

    InlineObjectEnumerator InlineObjects() const {
        return InlineObjectEnumerator{ inline_objects_.Items() };
    }

    std::shared_ptr<textual::InlineObject> GetInlineObjectAtIndex(std::size_t index) const {
        return inline_objects_.GetInlineObjectAtIndex(index);
    }

    std::vector<std::shared_ptr<textual::InlineObject>> AttachInlineObjectToRange(
        std::shared_ptr<textual::InlineObject> object,
        const Range& range) {

        return inline_objects_.Add(range, std::move(object));
    }

    std::vector<std::shared_ptr<textual::InlineObject>> ReplaceSpan(
        const Range& span_range, 
        std::size_t new_length) {

        fonts_.ReplaceSpan(span_range, new_length);
        color_pickers_.ReplaceSpan(span_range, new_length);
        return inline_objects_.ReplaceSpan(span_range, new_length);
    }

    void ClearFonts() {
        fonts_.Clear();
    }

    void ClearTextColorPickers() {
        color_pickers_.Clear();
    }

    std::vector<std::shared_ptr<textual::InlineObject>> ClearInlineObjects() {
        return inline_objects_.Clear();
    }

    std::vector<std::shared_ptr<textual::InlineObject>> Clear() {
        fonts_.Clear();
        color_pickers_.Clear();
        return inline_objects_.Clear();
    }

private:
    FontMap fonts_;
    ColorPickerMap color_pickers_;
    RangedInlineObjectCollection inline_objects_;
};

}