#include <zaf/control/textual_control.h>
#include <zaf/application.h>
#include <zaf/base/event_utility.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/renderer/renderer.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/resource_factory.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout_properties.h>
#include <zaf/internal/theme.h>

namespace zaf {

typedef internal::RangeMap<Font> FontRangeMap;
typedef internal::RangeMap<ColorPicker> TextColorPickerRangeMap;

static std::shared_ptr<FontRangeMap> TryGetFontRangeMap(const PropertyMap& property_map);
static std::shared_ptr<TextColorPickerRangeMap> TryGetTextColorPickersRangeMap(const PropertyMap& property_map);
static void SetFontToTextLayout(const Font& font, const TextRange& range, TextLayout& text_layout);

static const wchar_t* const kDefaultFontPropertyName = L"DefaultFont";
static const wchar_t* const kDefaultTextColorPickerPropertyName = L"DefaultTextColorPicker";
static const wchar_t* const kFontsPropertyName = L"Fonts";
static const wchar_t* const kParagraphAlignmentPropertyName = L"ParagraphAlignment";
static const wchar_t* const kTextAlignmentPropertyName = L"TextAlignment";
static const wchar_t* const kTextChangeEventPropertyName = L"TextChangeEvent";
static const wchar_t* const kTextColorPickersPropertyName = L"TextColorPickers";
static const wchar_t* const kTextPropertyName = L"Text";
static const wchar_t* const kWordWrappingPropertyName = L"WordWrapping";

TextualControl::TextualControl() {

}


TextualControl::~TextualControl() {

}


void TextualControl::Paint(Canvas& canvas, const Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    Rect text_rect = GetTextRect();
    if (text_rect.size.width <= 0 || text_rect.size.height <= 0) {
        return;
    }

    if (text_layout_ == nullptr) {
        text_layout_ = CreateTextLayout();
        if (text_layout_ == nullptr) {
            return;
        }
    }

    text_layout_.SetMaxWidth(text_rect.size.width);
    text_layout_.SetMaxHeight(text_rect.size.height);

    SetTextColorsToTextLayout(text_layout_, canvas.GetRenderer());

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(GetDefaultTextColor());
    canvas.DrawText(text_layout_, text_rect.position);
}


const TextLayout TextualControl::CreateTextLayout() {

    auto text_format = CreateTextFormat();
    if (text_format == nullptr) {
        return TextLayout();
    }

    TextLayoutProperties text_layout_properties;
    text_layout_properties.text = GetText();
    text_layout_properties.text_format = text_format;
    auto text_layout = GetResourceFactory()->CreateTextLayout(text_layout_properties);

    if (text_layout != nullptr) {
        SetFontsToTextLayout(text_layout);
    }

    return text_layout;
}


const TextFormat TextualControl::CreateTextFormat() const {

    auto font = GetDefaultFont();
    TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = font.family_name;
    text_format_properties.font_size = font.size;
    text_format_properties.font_weight = font.weight;

    auto text_format = GetResourceFactory()->CreateTextFormat(text_format_properties);
    if (text_format == nullptr) {
        return TextFormat();
    }

    text_format.SetTextAlignment(GetTextAlignment());
    text_format.SetParagraphAlignment(GetParagraphAlignment());
    text_format.SetWordWrapping(GetWordWrapping());

    return text_format;
}


void TextualControl::SetFontsToTextLayout(TextLayout& text_layout) {

    auto fonts = TryGetFontRangeMap(GetPropertyMap());
    if (fonts == nullptr) {
        return;
    }

    auto ranges_and_fonts = fonts->GetAllRangesAndValues();
    for (const auto& each_pair : ranges_and_fonts) {

        TextRange range(each_pair.first.first, each_pair.first.second);
        SetFontToTextLayout(each_pair.second, range, text_layout);
    }
}


void TextualControl::SetTextColorsToTextLayout(TextLayout& text_layout, Renderer& renderer) {

    auto text_color_pickers = TryGetTextColorPickersRangeMap(GetPropertyMap());
    if (text_color_pickers == nullptr) {
        return;
    }

    auto ranges_and_text_color_pickers = text_color_pickers->GetAllRangesAndValues();
    for (const auto& each_pair : ranges_and_text_color_pickers) {

        auto brush = renderer.CreateSolidColorBrush(each_pair.second(*this));
        text_layout.SetBrush(brush, TextRange(each_pair.first.first, each_pair.first.second));
    }
}


void TextualControl::ReleaseTextLayout() {
    text_layout_.Reset();
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

    if (text == GetText()) {
        return;
    }

    GetPropertyMap().SetProperty(kTextPropertyName, text);

    ReleaseTextLayout();
    NeedRepaint();
    NotifyTextChange();
}


const ColorPicker TextualControl::GetDefaultTextColorPicker() const {

    auto color_picker = GetPropertyMap().TryGetProperty<ColorPicker>(kDefaultTextColorPickerPropertyName);
    if ((color_picker != nullptr) && (*color_picker != nullptr)) {
        return *color_picker;
    }
    else {

        return [](const Control& control) {
            if (control.IsEnabled()) {
                return Color::FromRGB(internal::ControlNormalTextColorRGB);
            }
            else {
                return Color::FromRGB(internal::ControlDisabledTextColorRGB);
            }
        };
    }
}

void TextualControl::SetDefaultTextColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kDefaultTextColorPickerPropertyName, color_picker);
    NeedRepaint();
}


const ColorPicker TextualControl::GetTextColorPickerAtPosition(std::size_t position) const {

    auto color_pickers = TryGetTextColorPickersRangeMap(GetPropertyMap());
    if (color_pickers != nullptr) {

        auto color_picker = color_pickers->GetValueAtPosition(position);
        if (color_picker != nullptr) {
            return color_picker;
        }
    }

    return GetDefaultTextColorPicker();
}


void TextualControl::SetTextColorPickerAtRange(const ColorPicker& color_picker, const TextRange& range) {

    auto color_pickers = GetPropertyMap().GetProperty<std::shared_ptr<TextColorPickerRangeMap>>(
        kTextColorPickersPropertyName, 
        []() { return std::make_shared<TextColorPickerRangeMap>(); });

    if (color_picker == nullptr) {
        return;
    }

    color_pickers->AddValueToRange(color_picker, range.index, range.length);

    ReleaseTextLayout();
    NeedRepaint();
}


void TextualControl::ResetTextColorPickers() {

    auto color_pickers = TryGetTextColorPickersRangeMap(GetPropertyMap());
    if (color_pickers == nullptr) {
        return;
    }

    color_pickers->RemoveAllValues();

    ReleaseTextLayout();
    NeedRepaint();
}


const Font TextualControl::GetDefaultFont() const {

    auto font = GetPropertyMap().TryGetProperty<Font>(kDefaultFontPropertyName);
    if (font != nullptr) {
        return *font;
    }
    else {
        return Font::GetDefault();
    };
}

void TextualControl::SetDefaultFont(const Font& font) {

    GetPropertyMap().SetProperty(kDefaultFontPropertyName, font);

    ReleaseTextLayout(); 
    NeedRepaint();
}


const Font TextualControl::GetFontAtPosition(std::size_t position) const {
    
    auto fonts = TryGetFontRangeMap(GetPropertyMap());
    if (fonts != nullptr) {
            
        bool is_existent = false;
        auto font = fonts->GetValueAtPosition(position, &is_existent);

        if (is_existent) {
            return font;
        }
    }

    return GetDefaultFont();
}


void TextualControl::SetFontAtRange(const Font& font, const TextRange& range) {

    auto fonts = GetPropertyMap().GetProperty<std::shared_ptr<FontRangeMap>>(
        kFontsPropertyName,
        []() { return std::make_shared<FontRangeMap>(); });

    if (fonts == nullptr) {
        return;
    }

    fonts->AddValueToRange(font, range.index, range.length);

    if (text_layout_ != nullptr) {
        SetFontToTextLayout(font, range, text_layout_);
    }

    NeedRepaint();
}


void TextualControl::ResetFonts() {

    auto fonts = TryGetFontRangeMap(GetPropertyMap());
    if (fonts == nullptr) {
        return;
    }

    fonts->RemoveAllValues();

    ReleaseTextLayout();
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

    if (text_layout_ != nullptr) {
        text_layout_.SetTextAlignment(alignment);
    }

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

    if (text_layout_ != nullptr) {
        text_layout_.SetParagraphAlignment(alignment);
    }

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

    if (text_layout_ != nullptr) {
        text_layout_.SetWordWrapping(word_wrapping);
    }

    NeedRepaint();
}


TextualControl::TextChangeEvent::Proxy TextualControl::GetTextChangeEvent() {
    return GetEventProxyFromPropertyMap<TextChangeEvent>(GetPropertyMap(), kTextChangeEventPropertyName);
}


TextLayout TextualControl::CreateTextLayout(const Size& layout_size) const {

    auto text_format = CreateTextFormat();
    if (text_format == nullptr) {
        return TextLayout();
    }

    TextLayoutProperties text_layout_properties;
    text_layout_properties.text = GetText();
    text_layout_properties.text_format = text_format;
    text_layout_properties.width = layout_size.width;
    text_layout_properties.height = layout_size.height;
    return GetResourceFactory()->CreateTextLayout(text_layout_properties);
}


void TextualControl::NotifyTextChange() {

    auto event = TryGetEventFromPropertyMap<TextChangeEvent>(GetPropertyMap(), kTextChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(std::dynamic_pointer_cast<TextualControl>(shared_from_this()));
    }
}


static std::shared_ptr<FontRangeMap> TryGetFontRangeMap(const PropertyMap& property_map) {

    auto fonts_pointer = property_map.TryGetProperty<std::shared_ptr<FontRangeMap>>(kFontsPropertyName);
    if (fonts_pointer != nullptr) {
        return *fonts_pointer;
    }
    else {
        return nullptr;
    }
}


static std::shared_ptr<TextColorPickerRangeMap> TryGetTextColorPickersRangeMap(const PropertyMap& property_map) {

    auto color_pickers_pointer = property_map.TryGetProperty<std::shared_ptr<TextColorPickerRangeMap>>(kTextColorPickersPropertyName);
    if (color_pickers_pointer != nullptr) {
        return *color_pickers_pointer;
    }
    else {
        return nullptr;
    }
}


static void SetFontToTextLayout(const Font& font, const TextRange& range, TextLayout& text_layout) {

    text_layout.SetFontFamilyName(font.family_name, range);
    text_layout.SetFontSize(font.size, range);
    text_layout.SetFontWeight(font.weight, range);
}


}