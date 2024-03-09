#pragma once

#include <zaf/base/error/check.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/enumerator.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/graphic/font/font.h>
#include <zaf/internal/textual/inline_object_collection.h>

namespace zaf::internal {

class RangedTextStyle : NonCopyable {
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
        explicit InlineObjectItem(const InlineObjectCollection::ItemList::value_type& item) : 
            inner_(item) { }
        const Range& Range() const { return inner_.Range(); }
        const std::shared_ptr<textual::InlineObject>& InlineObject() const {
            return inner_.Object();
        }
    private:
        const InlineObjectCollection::ItemList::value_type& inner_;
    };

    using InlineObjectEnumerator = WrapEnumerator<
        InlineObjectCollection::ItemList,
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

    void AttachInlineObjectToRange(
        std::shared_ptr<textual::InlineObject> object,
        const Range& range) {

        inline_objects_.Add(range, std::move(object));
    }

    void ReplaceSpan(
        const Range& span_range, 
        std::size_t new_length) {

        fonts_.ReplaceSpan(span_range, new_length);
        color_pickers_.ReplaceSpan(span_range, new_length);
        inline_objects_.ReplaceSpan(span_range, new_length);
    }

    void ClearFonts() {
        fonts_.Clear();
    }

    void ClearTextColorPickers() {
        color_pickers_.Clear();
    }

    void ClearInlineObjects() {
        inline_objects_.Clear();
    }

    void Clear() {
        fonts_.Clear();
        color_pickers_.Clear();
        inline_objects_.Clear();
    }

    Observable<InlineObjectChangedInfo> InlineObjectChangedEvent() const {
        return inline_objects_.ChangedEvent();
    }

private:
    FontMap fonts_;
    ColorPickerMap color_pickers_;
    InlineObjectCollection inline_objects_;
};

}