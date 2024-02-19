#include <zaf/control/textual_control.h>
#include <zaf/base/as.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/internal/theme.h>
#include <zaf/object/type_definition.h>

namespace zaf {
namespace {

void ReviseTextTrimmingSign(TextTrimming& text_trimming, const TextFormat& text_format) {
    if (text_trimming.Granularity() != TextTrimmingGranularity::None) {
        if (!text_trimming.Sign()) {
            text_trimming.SetSign(
                GraphicFactory::Instance().CreateEllipsisTrimmingSign(text_format));
        }
    }
}


void SetFontToTextLayout(const Font& font, const Range& range, TextLayout& text_layout) {

    text_layout.SetFontFamilyName(font.family_name, range);
    text_layout.SetFontSize(font.size, range);
    text_layout.SetFontWeight(font.weight, range);
    text_layout.SetFontStyle(font.style, range);
    text_layout.SetHasUnderline(font.has_underline, range);
}

}

ZAF_DEFINE_TYPE(TextualControl)
ZAF_DEFINE_TYPE_PROPERTY(IgnoreTailingWhiteSpaces)
ZAF_DEFINE_TYPE_PROPERTY(Text)
ZAF_DEFINE_TYPE_PROPERTY(TextLength)
ZAF_DEFINE_TYPE_PROPERTY(TextColor)
ZAF_DEFINE_TYPE_PROPERTY(Font)
ZAF_DEFINE_TYPE_PROPERTY(FontFamily)
ZAF_DEFINE_TYPE_PROPERTY(FontSize)
ZAF_DEFINE_TYPE_PROPERTY(FontWeight)
ZAF_DEFINE_TYPE_PROPERTY(TextAlignment)
ZAF_DEFINE_TYPE_PROPERTY(ParagraphAlignment)
ZAF_DEFINE_TYPE_PROPERTY(WordWrapping)
ZAF_DEFINE_TYPE_END


TextualControl::TextualControl() {
    
}


TextualControl::~TextualControl() {

}


void TextualControl::Initialize() {

    __super::Initialize();

    Subscriptions() += text_model_.TextChangedEvent().Subscribe(
        std::bind(&TextualControl::OnTextModelChanged, this, std::placeholders::_1));
}


void TextualControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);
    
    auto text_rect = DetermineTextRect();
    UpdateTextRect(text_rect);
}


void TextualControl::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    //No need to paint if layout rect is empty.
    if (text_rect_.size.width <= 0 || text_rect_.size.height <= 0) {
        return;
    }

    auto content_rect = this->ContentRect();

    auto layout_rect_in_control = text_rect_;
    layout_rect_in_control.AddOffset(content_rect.position);

    auto text_boundary = Rect::Intersect(layout_rect_in_control, content_rect);
    auto clipping_guard = canvas.PushClipping(text_boundary);

    auto text_layout = GetTextLayout();
    SetTextColorsToTextLayout(text_layout, canvas.Renderer());

    PaintTextLayout(canvas, dirty_rect, text_layout, layout_rect_in_control);
}


void TextualControl::SetTextColorsToTextLayout(TextLayout& text_layout, Renderer& renderer) const {

    for (const auto& each_item : ranged_text_color_picker_) {
        auto brush = renderer.CreateSolidColorBrush(each_item.Value()(*this));
        text_layout.SetBrush(brush, each_item.Range());
    }
}


void TextualControl::PaintTextLayout(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const TextLayout& text_layout,
    const zaf::Rect& layout_rect) {

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(TextColorPicker()(*this));
    canvas.DrawTextLayout(text_layout, layout_rect.position);
}


void TextualControl::ReleaseRendererResources() {
    ReleaseTextLayout();
}


zaf::Rect TextualControl::DetermineTextRect() {
    return zaf::Rect{ zaf::Point{}, ContentSize() };
}


void TextualControl::UpdateTextRect(const zaf::Rect& text_rect) {

    text_rect_ = text_rect;
    text_rect_.size.width = (std::max)(text_rect_.size.width, 0.f);
    text_rect_.size.height = (std::max)(text_rect_.size.height, 0.f);

    if (text_layout_) {
        text_layout_.SetMaxWidth(text_rect_.size.width);
        text_layout_.SetMaxHeight(text_rect_.size.height);
    }
}


std::size_t TextualControl::TextLength() const {
    return text_model_.GetText().length();
}


std::wstring TextualControl::Text() const {
    return std::wstring{ text_model_.GetText() };
}


void TextualControl::SetText(const std::wstring& text) {
    
    if (text_model_.GetText() != text) {
        text_model_.SetText(text);
    }
}


ColorPicker TextualControl::TextColorPicker() const {

    if (default_text_color_picker_) {
        return default_text_color_picker_;
    }

    return [](const Control& control) {
        if (control.IsEnabledInContext()) {
            return Color::FromRGB(internal::ControlNormalTextColorRGB);
        }
        else {
            return Color::FromRGB(internal::ControlDisabledTextColorRGB);
        }
    };
}


void TextualControl::SetTextColorPicker(const ColorPicker& color_picker) {

    default_text_color_picker_ = color_picker;
    ReleaseTextLayout();
    NeedRepaint();
}


ColorPicker TextualControl::GetTextColorPickerAtIndex(std::size_t index) const {

    auto color_picker = ranged_text_color_picker_.GetValueAtIndex(index);
    if (color_picker) {
        return *color_picker;
    }

    return TextColorPicker();
}


void TextualControl::SetTextColorPickerInRange(
    const ColorPicker& color_picker, 
    const Range& range) {

    ranged_text_color_picker_.AddRange(range, color_picker);

    ReleaseTextLayout();
    NeedRepaint();
}


void TextualControl::ResetTextColorPickers() {

    ranged_text_color_picker_.Clear();

    ReleaseTextLayout();
    NeedRepaint();
}


Font TextualControl::Font() const {
    return default_font_;
}

void TextualControl::SetFont(const zaf::Font& font) {
    if (font != Font()) {
        InnerSetFont(font);
    }
}


void TextualControl::InnerSetFont(const zaf::Font& new_font) {

    default_font_ = new_font;

    ReleaseTextLayout();
    RaiseContentChangedEvent();
    NeedRepaint();
}


std::wstring TextualControl::FontFamily() const {
    return Font().family_name;
}

void TextualControl::SetFontFamily(const std::wstring& family) {
    auto font = Font();
    if (font.family_name != family) {
        font.family_name = family;
        InnerSetFont(font);
    }
}


float TextualControl::FontSize() const {
    return Font().size;
}

void TextualControl::SetFontSize(float size) {
    auto font = Font();
    if (font.size != size) {
        font.size = size;
        InnerSetFont(font);
    }
}


FontWeight TextualControl::FontWeight() const {
    return Font().weight;
}

void TextualControl::SetFontWeight(zaf::FontWeight weight) {
    auto font = Font();
    if (font.weight != weight) {
        font.weight = weight;
        InnerSetFont(font);
    }
}


Font TextualControl::GetFontAtIndex(std::size_t index) const {

    auto font = ranged_font_.GetValueAtIndex(index);
    if (font) {
        return *font;
    }
    return default_font_;
}


void TextualControl::SetFontInRange(const zaf::Font& font, const Range& range) {

    ranged_font_.AddRange(range, font);

    if (text_layout_) {
        SetFontToTextLayout(font, range, text_layout_);
    }

    NeedRepaint();
}


void TextualControl::ResetFonts() {

    ranged_font_.Clear();

    ReleaseTextLayout();
    NeedRepaint();
}


zaf::TextAlignment TextualControl::TextAlignment() const {
    return text_alignment_;
}

void TextualControl::SetTextAlignment(zaf::TextAlignment alignment) {

    text_alignment_ = alignment;

    if (text_layout_) {
        text_layout_.SetTextAlignment(alignment);
    }

    NeedRepaint();
}


zaf::ParagraphAlignment TextualControl::ParagraphAlignment() const {
    return paragraph_alignment_;
}

void TextualControl::SetParagraphAlignment(zaf::ParagraphAlignment alignment) {

    paragraph_alignment_ = alignment;

    if (text_layout_) {
        text_layout_.SetParagraphAlignment(alignment);
    }

    NeedRepaint();
}


WordWrapping TextualControl::WordWrapping() const {
    return word_wrapping_;
}

void TextualControl::SetWordWrapping(zaf::WordWrapping word_wrapping) {

    word_wrapping_ = word_wrapping;

    if (text_layout_) {
        text_layout_.SetWordWrapping(word_wrapping);
    }

    RaiseContentChangedEvent();
    NeedRepaint();
}


TextTrimming TextualControl::TextTrimming() const {
    return text_trimming_;
}

void TextualControl::SetTextTrimming(const zaf::TextTrimming& text_trimming) {

    text_trimming_ = text_trimming;

    if (text_layout_) {
        auto applied_text_trimming = text_trimming;
        ReviseTextTrimmingSign(applied_text_trimming, text_layout_);
        text_layout_.SetTextTrimming(applied_text_trimming);
    }

    NeedRepaint();
}


zaf::LineSpacing TextualControl::LineSpacing() const {
    return line_spacing_;
}

void TextualControl::SetLineSpacing(const zaf::LineSpacing& line_spacing) {

    line_spacing_ = line_spacing;

    if (text_layout_) {
        text_layout_.SetLineSpacing(line_spacing);
    }

    NeedRepaint();
}


bool TextualControl::IgnoreTailingWhiteSpaces() const {
    return ignore_tailing_white_spaces_;
}


void TextualControl::SetIgnoreTailingWhiteSpaces(bool value) {
    ignore_tailing_white_spaces_ = value;
}


Observable<TextChangedInfo> TextualControl::TextChangedEvent() const {
    return text_changed_event_.GetObservable();
}


void TextualControl::OnTextModelChanged(const internal::TextModelChangedInfo& event_info) {

    ranged_font_.EraseSpan(event_info.ReplacedRange());
    ranged_font_.InsertSpan(event_info.NewRange());

    ranged_text_color_picker_.EraseSpan(event_info.ReplacedRange());
    ranged_text_color_picker_.InsertSpan(event_info.NewRange());

    ReleaseTextLayout();

    OnTextChanged(TextChangedInfo{ As<TextualControl>(shared_from_this()) });
    RaiseContentChangedEvent();

    NeedRepaint();
}


void TextualControl::OnTextChanged(const TextChangedInfo& event_info) {
    text_changed_event_.Raise(event_info);
}


zaf::Size TextualControl::CalculatePreferredContentSize(const zaf::Size& max_size) const {

    auto text_layout = GetTextLayout();

    //Set layout size to boundary size for calculation.
    text_layout.SetMaxWidth(max_size.width);
    text_layout.SetMaxHeight(max_size.height);

    auto metrics = text_layout.GetMetrics();

    //Recover layout size.
    text_layout.SetMaxWidth(text_rect_.size.width);
    text_layout.SetMaxHeight(text_rect_.size.height);

    return zaf::Size{ metrics.GetWidth(ignore_tailing_white_spaces_), metrics.Height() };
}


TextLayout TextualControl::GetTextLayout() const {

    if (!text_layout_) {
        text_layout_ = CreateTextLayout();
    }

    return text_layout_;
}


TextLayout TextualControl::CreateTextLayout() const {

    auto text = text_model_.GetText();

    auto text_layout = GraphicFactory::Instance().CreateTextLayout(
        text,
        CreateTextFormat(),
        text_rect_.size);

    if (default_font_.has_underline) {
        Range range{ 0, text.length() };
        text_layout.SetHasUnderline(true, range);
    }

    for (const auto& each_item : ranged_font_) {
        SetFontToTextLayout(each_item.Value(), each_item.Range(), text_layout);
    }

    return text_layout;
}


TextFormat TextualControl::CreateTextFormat() const {

    TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = default_font_.family_name;
    text_format_properties.font_size = default_font_.size;
    text_format_properties.font_weight = default_font_.weight;
    text_format_properties.font_style = default_font_.style;

    auto text_format = GraphicFactory::Instance().CreateTextFormat(text_format_properties);
    text_format.SetTextAlignment(text_alignment_);
    text_format.SetParagraphAlignment(paragraph_alignment_);
    text_format.SetWordWrapping(word_wrapping_);
    text_format.SetLineSpacing(line_spacing_);

    auto text_trimming = text_trimming_;
    ReviseTextTrimmingSign(text_trimming, text_format);
    text_format.SetTextTrimming(text_trimming);

    return text_format;
}


void TextualControl::ReleaseTextLayout() {
    text_layout_ = {};
}

}