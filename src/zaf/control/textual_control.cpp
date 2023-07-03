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


void TextualControl::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) {

    __super::Paint(canvas, dirty_rect);

    zaf::Rect text_rect = DetermineTextRect();
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

    core_->PaintText(canvas, dirty_rect, text_rect);
}


void TextualControl::ReleaseRendererResources() {
    core_->ReleaseRendererResources();
}


TextLayout TextualControl::GetTextLayout() const {
    return dynamic_cast<internal::GeneralTextualCore*>(core_.get())->GetTextLayout();
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
    core_->SetFont(font);
}


float TextualControl::FontSize() const {
    return Font().size;
}

void TextualControl::SetFontSize(float size) {

    auto font = Font();
    if (font.size == size) {
        return;
    }

    font.size = size;
    SetFont(font);
}


FontWeight TextualControl::FontWeight() const {
    return Font().weight;
}

void TextualControl::SetFontWeight(zaf::FontWeight weight) {

    auto font = Font();
    if (font.weight == weight) {
        return;
    }

    font.weight = weight;
    SetFont(font);
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
    if (!text_layout) {
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

}