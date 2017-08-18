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
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

namespace zaf {

typedef internal::RangeMap<Font> FontRangeMap;
typedef internal::RangeMap<ColorPicker> TextColorPickerRangeMap;

static std::shared_ptr<FontRangeMap> TryGetFontRangeMap(const PropertyMap& property_map);
static std::shared_ptr<TextColorPickerRangeMap> TryGetTextColorPickersRangeMap(const PropertyMap& property_map);
static void SetFontToTextLayout(const Font& font, const TextRange& range, TextLayout& text_layout);

static const wchar_t* const kDefaultFontPropertyName = L"DefaultFont";
static const wchar_t* const kDefaultTextColorPickerPropertyName = L"DefaultTextColorPicker";
static const wchar_t* const kFontsPropertyName = L"Fonts";
static const wchar_t* const kTextChangeEventPropertyName = L"TextChangeEvent";
static const wchar_t* const kTextColorPickersPropertyName = L"TextColorPickers";
static const wchar_t* const kTextTrimmingPropertyName = L"TextTrimming";

namespace {

void ReviseTextTrimmingSign(TextTrimming& text_trimming, const TextFormat& text_format) {
    if (text_trimming.granularity != TextTrimming::Granularity::None) {
        if (text_trimming.trimming_sign == nullptr) {
            text_trimming.trimming_sign = GetResourceFactory()->CreateCreateEllipsisTrimmingSign(text_format);
        }
    }
}

}


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

    auto text_layout = GetTextLayout();
    if (text_layout == nullptr) {
        return;
    }

    text_layout.SetMaxWidth(text_rect.size.width);
    text_layout.SetMaxHeight(text_rect.size.height);

    SetTextColorsToTextLayout(text_layout, canvas.GetRenderer());

    Canvas::StateGuard state_guard(canvas);
    canvas.SetBrushWithColor(GetDefaultTextColor());
    canvas.PushClippingRect(text_rect);
    canvas.DrawTextLayout(text_layout, text_rect.position);
}


TextLayout TextualControl::CreateTextLayout() const {

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


TextFormat TextualControl::CreateTextFormat() const {

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

    auto text_trimming = GetTextTrimming();
    ReviseTextTrimmingSign(text_trimming, text_format);
    text_format.SetTextTrimming(text_trimming);

    return text_format;
}


void TextualControl::SetFontsToTextLayout(TextLayout& text_layout) const {

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


Rect TextualControl::GetTextRect() {
    return GetContentRect();
}


const std::wstring TextualControl::GetText() const {

    auto text = GetPropertyMap().TryGetProperty<std::wstring>(property::Text);
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

    GetPropertyMap().SetProperty(property::Text, text);

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

    auto text_alignment = GetPropertyMap().TryGetProperty<TextAlignment>(property::TextAlignment);
    if (text_alignment != nullptr) {
        return *text_alignment;
    }
    else {
        return TextAlignment::Leading;
    }
}

void TextualControl::SetTextAlignment(TextAlignment alignment) {

    GetPropertyMap().SetProperty(property::TextAlignment, alignment);

    if (text_layout_ != nullptr) {
        text_layout_.SetTextAlignment(alignment);
    }

    NeedRepaint();
}


ParagraphAlignment TextualControl::GetParagraphAlignment() const {

    auto paragraph_alignment = GetPropertyMap().TryGetProperty<ParagraphAlignment>(property::ParagraphAlignment);
    if (paragraph_alignment != nullptr) {
        return *paragraph_alignment;
    }
    else {
        return ParagraphAlignment::Near;
    }
}

void TextualControl::SetParagraphAlignment(ParagraphAlignment alignment) {

    GetPropertyMap().SetProperty(property::ParagraphAlignment, alignment);

    if (text_layout_ != nullptr) {
        text_layout_.SetParagraphAlignment(alignment);
    }

    NeedRepaint();
}


WordWrapping TextualControl::GetWordWrapping() const {

    auto word_wrapping = GetPropertyMap().TryGetProperty<WordWrapping>(property::WordWrapping);
    if (word_wrapping != nullptr) {
        return *word_wrapping;
    }
    else {
        return WordWrapping::NoWrap;
    }
}

void TextualControl::SetWordWrapping(WordWrapping word_wrapping) {

    GetPropertyMap().SetProperty(property::WordWrapping, word_wrapping);

    if (text_layout_ != nullptr) {
        text_layout_.SetWordWrapping(word_wrapping);
    }

    NeedRepaint();
}


TextTrimming TextualControl::GetTextTrimming() const {

    auto text_trimming = GetPropertyMap().TryGetProperty<TextTrimming>(kTextTrimmingPropertyName);
    if (text_trimming != nullptr) {
        return *text_trimming;
    }

    return TextTrimming();
}

void TextualControl::SetTextTrimming(const TextTrimming& text_trimming) {

    GetPropertyMap().SetProperty(kTextTrimmingPropertyName, text_trimming);

    if (text_layout_ != nullptr) {

        auto applied_text_trimming = text_trimming;
        ReviseTextTrimmingSign(applied_text_trimming, text_layout_);
        text_layout_.SetTextTrimming(applied_text_trimming);
    }

    NeedRepaint();
}


TextualControl::TextChangeEvent::Proxy TextualControl::GetTextChangeEvent() {
    return GetEventProxyFromPropertyMap<TextChangeEvent>(GetPropertyMap(), kTextChangeEventPropertyName);
}


void TextualControl::NotifyTextChange() {

    TextChange();

    auto event = TryGetEventFromPropertyMap<TextChangeEvent>(GetPropertyMap(), kTextChangeEventPropertyName);
    if (event != nullptr) {
        event->Trigger(std::dynamic_pointer_cast<TextualControl>(shared_from_this()));
    }
}


Size TextualControl::DetermineRequiredSize(const Size& max_size) const {

    auto text_layout = GetTextLayout();
    if (text_layout == nullptr) {
        return Size();
    }

    const auto& padding = GetPadding();
    const auto& border = GetBorder();
    float extract_width = padding.left + padding.right + border.left + border.right;
    float extract_height = padding.top + padding.bottom + border.top + border.bottom;
    
    text_layout.SetMaxWidth(max_size.width - extract_width);
    text_layout.SetMaxHeight(max_size.height - extract_height);
    auto metrics = text_layout.GetMetrics();
    return Size(metrics.width + extract_width, metrics.height + extract_height);
}


void TextualControl::DeserializeProperty(const std::wstring& name, const DataNode& data_node) {

    if (name == property::Text) {
        SetText(data_node.GetString());
    }
    else if (name == property::TextAlignment) {
        auto text_alignment = ConvertTextAlignmentFromString(data_node.GetString());
        SetTextAlignment(text_alignment.has_value() ? text_alignment.value() : TextAlignment::Leading);
    }
    else if (name == property::ParagraphAlignment) {
        auto paragraph_alignment = ConvertParagraphAlignmentFromString(data_node.GetString());
        SetParagraphAlignment(paragraph_alignment.has_value() ? paragraph_alignment.value() : ParagraphAlignment::Near);
    }
    else if (name == property::WordWrapping) {
        auto word_wrapping = ConvertWordWrappingFromString(data_node.GetString());
        SetWordWrapping(word_wrapping.has_value() ? word_wrapping.value() : WordWrapping::NoWrap);
    }
    else if (name == property::DefaultTextColor) {
        Color color;
        color.Deserialize(data_node);
        SetDefaultTextColor(color);
    }
    else if (name == property::DefaultTextColorPicker) {
        ConstantColorPicker color_picker;
        color_picker.Deserialize(data_node);
        SetDefaultTextColorPicker(color_picker);
    }
    else if (name == property::DefaultFont) {
        Font font;
        font.Deserialize(data_node);
        SetDefaultFont(font);
    }
    else if (name == property::TextTrimming) {
        TextTrimming text_trimming;
        text_trimming.Deserialize(data_node);
        SetTextTrimming(text_trimming);
    }
    else {
        __super::DeserializeProperty(name, data_node);
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


ZAF_DEFINE_TYPE_NAME(TextualControl);

}