#pragma once

#include <zaf/graphic/color.h>
#include <zaf/graphic/font.h>
#include <zaf/xml/xml_serializable.h>

namespace zaf::textual {

class DefaultTextStyle : public XMLSerializable {
public:
    DefaultTextStyle() : 
        font_(zaf::Font::Default()),
        text_color_(Color::Black()),
        text_back_color_(Color::Transparent()) {

    }

    const zaf::Font& Font() const noexcept {
        return font_;
    }

    void SetFont(zaf::Font font) noexcept {
        font_ = std::move(font);
    }

    const Color& TextColor() const noexcept {
        return text_color_;
    }

    void SetTextColor(const Color& color) noexcept {
        text_color_ = color;
    }

    const Color& TextBackColor() const noexcept {
        return text_back_color_;
    }

    void SetTextBackColor(const Color& color) noexcept {
        text_back_color_ = color;
    }

    void WriteToXML(XMLWriter& writer) const override;
    void ReadFromXML(XMLReader& reader) override;

private:
    zaf::Font font_;
    Color text_color_;
    Color text_back_color_;
};

}