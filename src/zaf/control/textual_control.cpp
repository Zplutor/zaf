#include <zaf/control/textual_control.h>
#include <zaf/application.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/renderer.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/renderer_factory.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout_properties.h>
#include <zaf/internal/theme.h>

namespace zaf {

static const wchar_t* const kFontPropertyName = L"Font";
static const wchar_t* const kParagraphAlignmentPropertyName = L"ParagraphAlignment";
static const wchar_t* const kTextAlignmentPropertyName = L"TextAlignment";
static const wchar_t* const kTextColorPickerPropertyName = L"TextColorPicker";
static const wchar_t* const kTextPropertyName = L"Text";
static const wchar_t* const kWordWrappingPropertyName = L"WordWrapping";

TextualControl::TextualControl() {

}


TextualControl::~TextualControl() {

}


void TextualControl::Initialize() {

    __super::Initialize();

    SetTextColorPicker([](const Control& control) {

        if (control.IsEnabled()) {
            return internal::ControlNormalTextColor;
        }
        else {
            return internal::ControlDisabledTextColor;
        }
    });
}


void TextualControl::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    Rect text_rect = GetTextRect();
    if (text_rect.IsEmpty()) {
        return;
    }

    auto text_layout = CreateTextLayout(text_rect.size);
    if (text_layout == nullptr) {
        return;
    }

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(GetTextColor());
    canvas.DrawText(text_layout, text_rect.position);
}


const Rect TextualControl::GetTextRect() const {
    return GetContentRect();
}


const std::wstring TextualControl::GetText() const {

    auto text = GetPropertyMap().TryGetProperty<std::wstring>(kTextPropertyName);
    if (text != nullptr) {
        return *text;
    }
    else {
        return std::wstring();
    }
}

void TextualControl::SetText(const std::wstring& text) {

    GetPropertyMap().SetProperty(kTextPropertyName, text);
    NeedRepaint();
}


const ColorPicker TextualControl::GetTextColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kTextColorPickerPropertyName);
    if ((color_picker != nullptr) && (*color_picker != nullptr)) {
        return *color_picker;
    }
    else {
        return EmptyColorPicker;
    }
}

void TextualControl::SetTextColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kTextColorPickerPropertyName, color_picker);
    NeedRepaint();
}


const Font TextualControl::GetFont() const {

    auto font = GetPropertyMap().TryGetProperty<Font>(kFontPropertyName);
    if (font != nullptr) {
        return *font;
    }
    else {
        return Font::GetDefault();
    };
}

void TextualControl::SetFont(const Font& font) {

    GetPropertyMap().SetProperty(kFontPropertyName, font);
    NeedRepaint();
}


TextAlignment TextualControl::GetTextAlignment() const {

    auto text_alignment = GetPropertyMap().TryGetProperty<TextAlignment>(kTextAlignmentPropertyName);
    if (text_alignment != nullptr) {
        return *text_alignment;
    }
    else {
        return TextAlignment::Leading;
    }
}

void TextualControl::SetTextAlignment(TextAlignment alignment) {

    GetPropertyMap().SetProperty(kTextAlignmentPropertyName, alignment);
    NeedRepaint();
}


ParagraphAlignment TextualControl::GetParagraphAlignment() const {

    auto paragraph_alignment = GetPropertyMap().TryGetProperty<ParagraphAlignment>(kParagraphAlignmentPropertyName);
    if (paragraph_alignment != nullptr) {
        return *paragraph_alignment;
    }
    else {
        return ParagraphAlignment::Near;
    }
}

void TextualControl::SetParagraphAlignment(ParagraphAlignment alignment) {

    GetPropertyMap().SetProperty(kParagraphAlignmentPropertyName, alignment);
    NeedRepaint();
}


WordWrapping TextualControl::GetWordWrapping() const {

    auto word_wrapping = GetPropertyMap().TryGetProperty<WordWrapping>(kWordWrappingPropertyName);
    if (word_wrapping != nullptr) {
        return *word_wrapping;
    }
    else {
        return WordWrapping::NoWrap;
    }
}

void TextualControl::SetWordWrapping(WordWrapping word_wrapping) {

    GetPropertyMap().SetProperty(kWordWrappingPropertyName, word_wrapping);
    NeedRepaint();
}


std::shared_ptr<TextFormat> TextualControl::CreateTextFormat() const {

    auto font = GetFont();
    TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = font.family_name;
    text_format_properties.font_size = font.size;
    text_format_properties.font_weight = font.weight;

    auto text_format = Application::GetInstance().GetRendererFactory()->CreateTextFormat(text_format_properties);
    if (text_format == nullptr) {
        return nullptr;
    }

    text_format->SetTextAlignment(GetTextAlignment());
    text_format->SetParagraphAlignment(GetParagraphAlignment());
    text_format->SetWordWrapping(GetWordWrapping());

    return text_format;
}


std::shared_ptr<TextLayout> TextualControl::CreateTextLayout(const Size& layout_size) const {

    auto text_format = CreateTextFormat();
    if (text_format == nullptr) {
        return nullptr;
    }

    TextLayoutProperties text_layout_properties;
    text_layout_properties.text = GetText();
    text_layout_properties.text_format = text_format;
    text_layout_properties.width = layout_size.width;
    text_layout_properties.height = layout_size.height;
    return Application::GetInstance().GetRendererFactory()->CreateTextLayout(text_layout_properties);
}


}