#pragma once

#include <optional>
#include <zaf/control/textual/inline_object.h>
#include <zaf/graphic/color.h>
#include <zaf/graphic/font/font.h>

namespace zaf::textual {

class TextStyle {
public:
    const zaf::Font* Font() const {
        if (font_) {
            return &*font_;
        }
        return nullptr;
    }

    zaf::Font* Font() {
        if (font_) {
            return &*font_;
        }
        return nullptr;
    }

    void SetFont(zaf::Font font) {
        font_ = std::move(font);
    }

    const Color* TextColor() const {
        if (text_color_) {
            return &*text_color_;
        }
        return nullptr;
    }

    void SetTextColor(const Color& color) {
        text_color_ = color;
    }

    const Color* TextBackColor() const {
        if (text_back_color_) {
            return &*text_back_color_;
        }
        return nullptr;
    }

    void SetTextBackColor(const Color& color) {
        text_back_color_ = color;
    }

    const std::shared_ptr<textual::InlineObject>& InlineObject() const {
        return inline_object_;
    }

    void SetInlineObject(std::shared_ptr<textual::InlineObject> inline_object) {
        inline_object_ = std::move(inline_object);
    }

private:
    std::optional<zaf::Font> font_;
    std::optional<Color> text_color_;
    std::optional<Color> text_back_color_;
    std::shared_ptr<textual::InlineObject> inline_object_;
};

}