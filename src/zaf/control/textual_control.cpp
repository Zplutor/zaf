#include <zaf/control/textual_control.h>
#include <zaf/base/as.h>
#include <zaf/control/internal/textual_control/general_textual_core.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font/font.h>
#include <zaf/object/type_definition.h>

namespace zaf {

ZAF_DEFINE_TYPE(TextualControl)
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


TextualControl::TextualControl() : core_(std::make_unique<internal::GeneralTextualCore>()) {
    
}


TextualControl::TextualControl(std::unique_ptr<internal::TextualControlCore> core) : 
    core_(std::move(core)) {

}


TextualControl::~TextualControl() {

}


void TextualControl::Initialize() {

    __super::Initialize();

    core_->Initialize(As<TextualControl>(shared_from_this()));
}


void TextualControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);

    auto text_rect = DetermineTextRect();
    text_rect.size.width = (std::max)(text_rect.size.width, 0.f);
    text_rect.size.height = (std::max)(text_rect.size.height, 0.f);
    core_->LayoutText(text_rect);
}


void TextualControl::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);
    core_->PaintText(canvas, dirty_rect);
}


void TextualControl::ReleaseRendererResources() {
    core_->ReleaseRendererResources();
}


zaf::Rect TextualControl::DetermineTextRect() {
    return zaf::Rect{ zaf::Point{}, ContentSize() };
}


std::size_t TextualControl::TextLength() const {
    return core_->GetTextLength();
}


std::wstring TextualControl::Text() const {

    auto text = core_->GetText();
    auto string_view = std::get_if<std::wstring_view>(&text);
    if (string_view) {
        return std::wstring{ *string_view };
    }

    auto string = std::get_if<std::wstring>(&text);
    if (string) {
        return std::move(*string);
    }

    return {};
}


void TextualControl::SetText(const std::wstring& text) {

    auto result = core_->SetText(text);
    if (!result.is_changed) {
        return;
    }

    NeedRepaint();

    if (!result.is_notification_sent) {
        NotifyTextChanged();
    }
}


ColorPicker TextualControl::TextColorPicker() const {
    return core_->GetTextColorPicker();
}

void TextualControl::SetTextColorPicker(const ColorPicker& color_picker) {
    core_->SetTextColorPicker(color_picker);
}


ColorPicker TextualControl::GetTextColorPickerAtPosition(std::size_t position) const {
    return core_->GetTextColorPickerAtPosition(position);
}


void TextualControl::SetTextColorPickerAtRange(
    const ColorPicker& color_picker, 
    const Range& range) {

    core_->SetTextColorPickerAtRange(color_picker, range);
}


void TextualControl::ResetTextColorPickers() {
    core_->ResetTextColorPickers();
}


Font TextualControl::Font() const {
    return core_->GetFont();
}

void TextualControl::SetFont(const zaf::Font& font) {
    if (font != Font()) {
        InnerSetFont(font);
    }
}

void TextualControl::InnerSetFont(const zaf::Font& new_font) {
    core_->SetFont(new_font);
    RaiseContentChangedEvent();
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


Font TextualControl::GetFontAtPosition(std::size_t position) const {
    return core_->GetFontAtPosition(position);
}


void TextualControl::SetFontAtRange(const zaf::Font& font, const Range& range) {
    core_->SetFontAtRange(font, range);
}


void TextualControl::ResetFonts() {
    core_->ResetFonts();
}


TextAlignment TextualControl::TextAlignment() const {
    return core_->GetTextAlignment();
}

void TextualControl::SetTextAlignment(zaf::TextAlignment alignment) {
    core_->SetTextAlignment(alignment);
}


ParagraphAlignment TextualControl::ParagraphAlignment() const {
    return core_->GetParagraphAlignment();
}

void TextualControl::SetParagraphAlignment(zaf::ParagraphAlignment alignment) {
    core_->SetParagraphAlignment(alignment);
}


WordWrapping TextualControl::WordWrapping() const {
    return core_->GetWordWrapping();
}

void TextualControl::SetWordWrapping(zaf::WordWrapping word_wrapping) {
    return core_->SetWordWrapping(word_wrapping);
}


TextTrimming TextualControl::TextTrimming() const {
    return core_->GetTextTrimming();
}

void TextualControl::SetTextTrimming(const zaf::TextTrimming& text_trimming) {
    core_->SetTextTrimming(text_trimming);
}


zaf::LineSpacing TextualControl::LineSpacing() const {
    return core_->GetLineSpacing();
}

void TextualControl::SetLineSpacing(const zaf::LineSpacing& line_spacing) {
    core_->SetLineSpacing(line_spacing);
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
    return core_->CalculateTextSize(max_size);
}

}