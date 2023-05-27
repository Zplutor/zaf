#include <zaf/control/textual_control.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/control/text_source.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/renderer/renderer.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/graphic/text/text_layout_properties.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>
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
static const wchar_t* const kTextColorPickersPropertyName = L"TextColorPickers";
static const wchar_t* const kTextTrimmingPropertyName = L"TextTrimming";

namespace {

void ReviseTextTrimmingSign(TextTrimming& text_trimming, const TextFormat& text_format) {
    if (text_trimming.Granularity() != TextTrimmingGranularity::None) {
        if (text_trimming.Sign() == nullptr) {
            text_trimming.SetSign(
                GraphicFactory::Instance().CreateEllipsisTrimmingSign(text_format));
        }
    }
}


void SetDefaultFontOtherPropertiesToTextLayout(
    const Font& font,
    std::size_t text_length, 
    TextLayout& text_layout) {

    if (font.has_underline) {

        TextRange range{ 0, text_length };
        text_layout.SetHasUnderline(true, range);
    }
}

}

ZAF_DEFINE_TYPE(TextualControl)
ZAF_DEFINE_TYPE_PROPERTY(Text)
ZAF_DEFINE_TYPE_PROPERTY(TextLength)
ZAF_DEFINE_TYPE_PROPERTY(TextColor)
ZAF_DEFINE_TYPE_PROPERTY(Font)
ZAF_DEFINE_TYPE_PROPERTY(FontSize)
ZAF_DEFINE_TYPE_PROPERTY(FontWeight)
ZAF_DEFINE_TYPE_PROPERTY(TextAlignment)
ZAF_DEFINE_TYPE_PROPERTY(ParagraphAlignment)
ZAF_DEFINE_TYPE_PROPERTY(WordWrapping)
ZAF_DEFINE_TYPE_END


//Note: cannot use SetTextSource() to set initial text source because text changed event will be 
//raised.
TextualControl::TextualControl() : text_source_(TextSource::Default()) {
    
}


TextualControl::~TextualControl() {

}


void TextualControl::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    zaf::Rect text_rect = GetTextRect();
    if (text_rect.size.width <= 0 || text_rect.size.height <= 0) {
        return;
    }

    //Text rect is related to content rect's coordinate system, transfer it to control's coordinate 
    //system.
    auto content_rect = ContentRect();
    text_rect.position.x += content_rect.position.x;
    text_rect.position.y += content_rect.position.y;
    //Prevent text rect exceeds content rect.
    text_rect.size.width = (std::min)(text_rect.size.width, content_rect.size.width);
    text_rect.size.height = (std::min)(text_rect.size.height, content_rect.size.height);

    auto text_layout = GetTextLayout();
    if (text_layout == nullptr) {
        return;
    }

    text_layout.SetMaxWidth(text_rect.size.width);
    text_layout.SetMaxHeight(text_rect.size.height);

    SetTextColorsToTextLayout(text_layout, canvas.Renderer());

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(TextColor());
    auto clipping_guard = canvas.PushClipping(text_rect);
    canvas.DrawTextLayout(text_layout, text_rect.position);
}


TextLayout TextualControl::GetTextLayout() const {
    if (!text_layout_) {
        text_layout_ = CreateTextLayout();
    }
    return text_layout_;
}


TextLayout TextualControl::CreateTextLayout() const {

    auto default_font = Font();

    auto text = Text();
    auto text_length = text.length();

    TextLayoutProperties text_layout_properties;
    text_layout_properties.text = std::move(text);
    text_layout_properties.text_format = CreateTextFormat(default_font);
    auto text_layout = GraphicFactory::Instance().CreateTextLayout(text_layout_properties);

    SetDefaultFontOtherPropertiesToTextLayout(default_font, text_length, text_layout);
    SetRangedFontsToTextLayout(text_layout);

    return text_layout;
}


TextFormat TextualControl::CreateTextFormat(const zaf::Font& default_font) const {

    TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = default_font.family_name;
    text_format_properties.font_size = default_font.size;
    text_format_properties.font_weight = default_font.weight;

    auto text_format = GraphicFactory::Instance().CreateTextFormat(text_format_properties);
    text_format.SetTextAlignment(TextAlignment());
    text_format.SetParagraphAlignment(ParagraphAlignment());
    text_format.SetWordWrapping(WordWrapping());

    auto text_trimming = TextTrimming();
    ReviseTextTrimmingSign(text_trimming, text_format);
    text_format.SetTextTrimming(text_trimming);

    return text_format;
}


void TextualControl::SetRangedFontsToTextLayout(TextLayout& text_layout) const {

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


zaf::Rect TextualControl::GetTextRect() {
    return zaf::Rect{ zaf::Point{}, ContentSize() };
}


std::size_t TextualControl::TextLength() const {
    return text_source_->GetTextLength();
}


std::wstring TextualControl::Text() const {

    auto text = text_source_->GetText();
    auto string_view = std::get_if<std::wstring_view>(&text);
    if (string_view) {
        return std::wstring{ *string_view };
    }

    auto string = std::get_if<std::wstring>(&text);
    if (string) {
        return *string;
    }

    return {};
}


void TextualControl::SetText(const std::wstring& text) {

    auto result = text_source_->SetText(text);
    if (!result.IsChanged()) {
        return;
    }

    AfterTextChanged(!result.IsNotificationSent());
}


void TextualControl::SetTextSource(std::unique_ptr<TextSource> text_source) {

    ZAF_EXPECT(text_source);

    text_source_ = std::move(text_source);
    AfterTextChanged(true);
}


void TextualControl::AfterTextChanged(bool need_send_notification) {

    ReleaseTextLayout();
    NeedRepaint();

    if (need_send_notification) {
        NotifyTextChanged();
    }
}


ColorPicker TextualControl::TextColorPicker() const {

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

void TextualControl::SetTextColorPicker(const ColorPicker& color_picker) {

    GetPropertyMap().SetProperty(kDefaultTextColorPickerPropertyName, color_picker);
    NeedRepaint();
}


ColorPicker TextualControl::GetTextColorPickerAtPosition(std::size_t position) const {

    auto color_pickers = TryGetTextColorPickersRangeMap(GetPropertyMap());
    if (color_pickers != nullptr) {

        auto color_picker = color_pickers->GetValueAtPosition(position);
        if (color_picker != nullptr) {
            return color_picker;
        }
    }

    return TextColorPicker();
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


Font TextualControl::Font() const {

    auto font = GetPropertyMap().TryGetProperty<zaf::Font>(kDefaultFontPropertyName);
    if (font != nullptr) {
        return *font;
    }
    else {
        return Font::Default();
    };
}

void TextualControl::SetFont(const zaf::Font& font) {

    GetPropertyMap().SetProperty(kDefaultFontPropertyName, font);

    ReleaseTextLayout(); 
    NeedRepaint();
}


float TextualControl::FontSize() const {
    return Font().size;
}

void TextualControl::SetFontSize(float size) {

    auto new_font = Font();
    new_font.size = size;
    SetFont(new_font);
}


FontWeight TextualControl::FontWeight() const {
    return Font().weight;
}

void TextualControl::SetFontWeight(zaf::FontWeight weight) {

    auto new_font = Font();
    new_font.weight = weight;
    SetFont(new_font);
}


Font TextualControl::GetFontAtPosition(std::size_t position) const {
    
    auto fonts = TryGetFontRangeMap(GetPropertyMap());
    if (fonts != nullptr) {
            
        bool is_existent = false;
        auto font = fonts->GetValueAtPosition(position, &is_existent);

        if (is_existent) {
            return font;
        }
    }

    return Font();
}


void TextualControl::SetFontAtRange(const zaf::Font& font, const TextRange& range) {

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


TextAlignment TextualControl::TextAlignment() const {

    auto text_alignment = 
        GetPropertyMap().TryGetProperty<zaf::TextAlignment>(property::TextAlignment);

    if (text_alignment != nullptr) {
        return *text_alignment;
    }
    else {
        return TextAlignment::Leading;
    }
}

void TextualControl::SetTextAlignment(zaf::TextAlignment alignment) {

    GetPropertyMap().SetProperty(property::TextAlignment, alignment);

    if (text_layout_ != nullptr) {
        text_layout_.SetTextAlignment(alignment);
    }

    NeedRepaint();
}


ParagraphAlignment TextualControl::ParagraphAlignment() const {

    auto paragraph_alignment = 
        GetPropertyMap().TryGetProperty<zaf::ParagraphAlignment>(property::ParagraphAlignment);

    if (paragraph_alignment != nullptr) {
        return *paragraph_alignment;
    }
    else {
        return ParagraphAlignment::Near;
    }
}

void TextualControl::SetParagraphAlignment(zaf::ParagraphAlignment alignment) {

    GetPropertyMap().SetProperty(property::ParagraphAlignment, alignment);

    if (text_layout_ != nullptr) {
        text_layout_.SetParagraphAlignment(alignment);
    }

    NeedRepaint();
}


WordWrapping TextualControl::WordWrapping() const {

    auto word_wrapping = GetPropertyMap().TryGetProperty<zaf::WordWrapping>(property::WordWrapping);
    if (word_wrapping != nullptr) {
        return *word_wrapping;
    }
    else {
        return WordWrapping::NoWrap;
    }
}

void TextualControl::SetWordWrapping(zaf::WordWrapping word_wrapping) {

    GetPropertyMap().SetProperty(property::WordWrapping, word_wrapping);

    if (text_layout_ != nullptr) {
        text_layout_.SetWordWrapping(word_wrapping);
    }

    NeedRepaint();
}


TextTrimming TextualControl::TextTrimming() const {

    auto text_trimming = GetPropertyMap().TryGetProperty<zaf::TextTrimming>(
        kTextTrimmingPropertyName);

    if (text_trimming != nullptr) {
        return *text_trimming;
    }

    return zaf::TextTrimming();
}

void TextualControl::SetTextTrimming(const zaf::TextTrimming& text_trimming) {

    GetPropertyMap().SetProperty(kTextTrimmingPropertyName, text_trimming);

    if (text_layout_ != nullptr) {

        auto applied_text_trimming = text_trimming;
        ReviseTextTrimmingSign(applied_text_trimming, text_layout_);
        text_layout_.SetTextTrimming(applied_text_trimming);
    }

    NeedRepaint();
}


Observable<TextChangedInfo> TextualControl::TextChangedEvent() const {
    return text_changed_event_.GetObservable();
}


void TextualControl::NotifyTextChanged() {

    TextChangedInfo event_info{ As<TextualControl>(shared_from_this())};
    OnTextChanged(event_info);

    RaiseContentChangedEvent();
}


void TextualControl::OnTextChanged(const TextChangedInfo& event_info) {
    text_changed_event_.Raise(event_info);
}


zaf::Size TextualControl::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    auto text_layout = GetTextLayout();
    if (text_layout == nullptr) {
        return zaf::Size();
    }
    
    text_layout.SetMaxWidth(max_size.width);
    text_layout.SetMaxHeight(max_size.height);

    auto metrics = text_layout.GetMetrics();

    //Note that if TextAlignment is set to Center, WidthIncludingTrailingWhitespace would be 0.
    auto width =
        metrics.WidthIncludingTrailingWhitespace() != 0 ?
        metrics.WidthIncludingTrailingWhitespace() :
        metrics.Width();

    return zaf::Size{ width, metrics.Height() };
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
    text_layout.SetHasUnderline(font.has_underline, range);
}

}