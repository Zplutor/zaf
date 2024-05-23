#pragma once

#include <zaf/base/error/contract_error.h>
#include <zaf/control/color_picker.h>
#include <zaf/graphic/font/font.h>

namespace zaf::textual {

class DefaultTextStyle {
public:
    const zaf::Font& Font() const noexcept {
        return font_;
    }

    void SetFont(zaf::Font font) noexcept {
        font_ = std::move(font);
    }

    const ColorPicker& TextColorPicker() const {
        return text_color_picker_;
    }

    void SetTextColor(const Color& color) {
        text_color_picker_ = CreateColorPicker(color);
    }

    void SetTextColorPicker(ColorPicker picker) {
        ZAF_EXPECT(picker);
        text_color_picker_ = std::move(picker);
    }

    const ColorPicker& TextBackColorPicker() const noexcept {
        return text_back_color_picker_;
    }

    void SetTextBackColor(const Color& color) {
        text_back_color_picker_ = CreateColorPicker(color);
    }

    void SetTextBackColorPicker(ColorPicker picker) {
        ZAF_EXPECT(picker);
        text_back_color_picker_ = std::move(picker);
    }

private:
    zaf::Font font_;
    ColorPicker text_color_picker_;
    ColorPicker text_back_color_picker_;
};

}