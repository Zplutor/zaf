#pragma once

#include <optional>
#include <zaf/control/color_picker.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/graphic/font/font.h>

namespace zaf::textual {

class TextStyle {
public:
    const std::optional<zaf::Font>& Font() const {
        return font_;
    }

    void SetFont(zaf::Font font) {
        font_ = std::move(font);
    }

    const ColorPicker& TextColorPicker() const {
        return text_color_picker_;
    }

    void SetTextColor(const Color& color) {
        text_color_picker_ = CreateColorPicker(color);
    }

    void SetTextColorPicker(ColorPicker picker) {
        text_color_picker_ = std::move(picker);
    }

    const ColorPicker& TextBackColorPicker() const {
        return text_back_color_picker_;
    }

    void SetTextBackColor(const Color& color) {
        text_back_color_picker_ = CreateColorPicker(color);
    }

    void SetTextBackColorPicker(ColorPicker picker) {
        text_back_color_picker_ = std::move(picker);
    }

    const std::shared_ptr<textual::InlineObject>& InlineObject() const {
        return inline_object_;
    }

    void SetInlineObject(std::shared_ptr<textual::InlineObject> inline_object) {
        inline_object_ = std::move(inline_object);
    }

private:
    std::optional<zaf::Font> font_;
    ColorPicker text_color_picker_;
    ColorPicker text_back_color_picker_;
    std::shared_ptr<textual::InlineObject> inline_object_;
};

}