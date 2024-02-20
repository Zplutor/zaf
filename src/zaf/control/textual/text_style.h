#pragma once

#include <zaf/base/error/check.h>
#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/text/custom_text_inline_object.h>

namespace zaf {

class TextStyle {
public:
    using RangedFontVisitor = std::function<void(const Font&, const Range&)>;
    using RangedTextColorPickerVisitor = std::function<void(const ColorPicker&, const Range&)>;
    using RangedInlineObjectVisitor = std::function<void(
        const std::shared_ptr<CustomTextInlineObject>&, 
        const Range&
    )>;

public:
    const Font& DefaultFont() const {
        return default_font_;
    }

    void SetDefaultFont(Font font) {
        default_font_ = std::move(font);
    }

    void SetFontInRange(Font font, const Range& range) {
        ranged_fonts_.AddRange(range, std::move(font));
    }

    void ClearRangedFonts() {
        ranged_fonts_.Clear();
    }

    const Font& GetFontAtIndex(std::size_t index) const {
        auto font = ranged_fonts_.GetValueAtIndex(index);
        if (font) {
            return *font;
        }
        return default_font_;
    }

    const ColorPicker& DefaultTextColorPicker() const {
        return default_text_color_picker_;
    }

    void SetDefaultTextColorPicker(ColorPicker color_picker) {
        ZAF_EXPECT(color_picker);
        default_text_color_picker_ = std::move(color_picker);
    }

    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range) {
        ZAF_EXPECT(color_picker);
        ranged_text_color_pickers_.AddRange(range, std::move(color_picker));
    }

    void ClearRangedTextColorPickers() {
        ranged_text_color_pickers_.Clear();
    }

    const ColorPicker& GetTextColorPickerAtIndex(std::size_t index) const {
        auto picker = ranged_text_color_pickers_.GetValueAtIndex(index);
        if (picker) {
            return *picker;
        }
        return default_text_color_picker_;
    }

    void SetInlineObjectInRange(
        std::shared_ptr<CustomTextInlineObject> object,
        const Range& range) {

        ZAF_EXPECT(object);
        ranged_inline_objects_.AddRange(range, std::move(object));
    }

    void ClearRangedInlineObjects() {
        ranged_inline_objects_.Clear();
    }

    std::shared_ptr<CustomTextInlineObject> GetInlineObjectAtIndex(std::size_t index) const {

        auto object = ranged_inline_objects_.GetValueAtIndex(index);
        if (object) {
            return *object;
        }
        return nullptr;
    }

    void ResizeRange(const Range& range, std::size_t new_length) {

        Range new_range{ range.index, new_length };

        ranged_fonts_.EraseSpan(range);
        ranged_fonts_.InsertSpan(new_range);

        ranged_text_color_pickers_.EraseSpan(range);
        ranged_text_color_pickers_.InsertSpan(new_range);

        ranged_inline_objects_.EraseSpan(range);
        ranged_inline_objects_.InsertSpan(new_range);
    }

    void VisitRangedFonts(const RangedFontVisitor& visitor) const {
        for (const auto& each_item : ranged_fonts_) {
            visitor(each_item.Value(), each_item.Range());
        }
    }

    void VisitRangedTextColorPickers(const RangedTextColorPickerVisitor& visitor) const {
        for (const auto& each_item : ranged_text_color_pickers_) {
            visitor(each_item.Value(), each_item.Range());
        }
    }

    void VisitRangedInlineObjects(const RangedInlineObjectVisitor& visitor) const {
        for (const auto& each_item : ranged_inline_objects_) {
            visitor(each_item.Value(), each_item.Range());
        }
    }

private:
    Font default_font_{ Font::Default() };
    internal::RangeMap<Font> ranged_fonts_;
    
    ColorPicker default_text_color_picker_{ CreateColorPicker(Color::Black()) };
    internal::RangeMap<ColorPicker> ranged_text_color_pickers_;

    internal::RangeMap<std::shared_ptr<CustomTextInlineObject>> ranged_inline_objects_;
};

}