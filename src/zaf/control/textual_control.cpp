#include <zaf/control/textual_control.h>
#include <zaf/base/as.h>
#include <zaf/control/control_object.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/graphic/canvas.h>
#include <zaf/graphic/font.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/internal/graphic/text_inline_object_bridge.h>
#include <zaf/internal/graphic/text_inline_object_painter.h>
#include <zaf/graphic/dwrite/text_format_properties.h>
#include <zaf/internal/theme.h>

using namespace zaf::dwrite;

namespace zaf {
namespace {

void ReviseTextTrimmingSign(TextTrimming& text_trimming, const TextFormat& text_format) {
    if (text_trimming.Granularity() != dwrite::TextTrimmingGranularity::None) {
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

ZAF_OBJECT_IMPL(TextualControl);


TextualControl::TextualControl() : 
    text_model_(std::make_unique<internal::TextModel>()),
    inline_object_painter_(std::make_shared<internal::TextInlineObjectPainter>()) {
    
}


TextualControl::~TextualControl() {

    //Release text model first to detach inline objects in advance.
    text_model_.reset();
}


void TextualControl::Initialize() {

    __super::Initialize();

    SetTextColorPicker(ColorPicker([](const Control& control) {
        if (control.IsEnabledInContext()) {
            return Color::FromRGB(internal::ControlNormalTextColorRGB);
        }
        else {
            return Color::FromRGB(internal::ControlDisabledTextColorRGB);
        }
    }));

    Subscriptions() += text_model_->InlineObjectAttachedEvent().Subscribe(
        std::bind(&TextualControl::OnInlineObjectAttached, this, std::placeholders::_1));

    Subscriptions() += text_model_->TextChangedEvent().Subscribe(
        std::bind(&TextualControl::OnTextModelChanged, this, std::placeholders::_1));
}


void TextualControl::Layout(const zaf::Rect& previous_rect) {

    __super::Layout(previous_rect);
    
    auto text_rect = DetermineTextRect();
    UpdateTextRect(text_rect);
}


void TextualControl::UpdateStyle() {

    __super::UpdateStyle();

    text_model_->UpdateColors(*this);
}


void TextualControl::Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const {

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

    //Paint text background.
    PaintTextBack(canvas, dirty_rect, text_layout, layout_rect_in_control);

    //Paint text.
    auto inline_object_guard = inline_object_painter_->BeginPaint(canvas);
    PaintText(canvas, dirty_rect, text_layout, layout_rect_in_control);
}


void TextualControl::SetTextColorsToTextLayout(
    TextLayout& text_layout,
    d2d::Renderer& renderer) const {

    for (const auto& each_item : text_model_->StyledText().RangedTextColors()) {

        auto brush = renderer.CreateSolidColorBrush(each_item.Value());
        text_layout.SetBrush(brush, each_item.Range());
    }
}


void TextualControl::PaintTextBack(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const TextLayout& text_layout,
    const zaf::Rect& layout_rect) const {

    auto& text_back_colors = text_model_->StyledText().RangedTextBackColors();
    if (text_back_colors.IsEmpty()) {
        return;
    }

    auto region_guard = canvas.PushRegion(layout_rect, layout_rect);

    for (const auto& each_item : text_back_colors) {
        PaintTextBackInRange(canvas, text_layout, each_item.Range(), each_item.Value());
    }
}


void TextualControl::PaintTextBackInRange(
    Canvas& canvas,
    const TextLayout& text_layout,
    const Range& range,
    const Color& color) const {

    auto range_metrics = text_layout.HitTestRange(range);
    for (const auto& metrics : range_metrics) {

        auto rect = metrics.Rect();
        rect.Deflate(text_back_padding_);
        canvas.DrawRectangle(rect, color);
    }
}


void TextualControl::PaintText(
    Canvas& canvas,
    const zaf::Rect& dirty_rect,
    const TextLayout& text_layout,
    const zaf::Rect& layout_rect) const {

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(TextColor());
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
    return text_model_->Text().length();
}


const std::wstring& TextualControl::Text() const {
    return text_model_->Text();
}


void TextualControl::SetText(std::wstring text) {
    text_model_->SetText(std::move(text));
}


void TextualControl::SetTextInRange(std::wstring_view text, const Range& range) {
    text_model_->SetTextInRange(text, range);
}


const textual::StyledText& TextualControl::StyledText() const noexcept {
    return text_model_->StyledText();
}

void TextualControl::SetStyledText(textual::StyledText styled_text) {
    text_model_->SetStyledText(std::move(styled_text));
}


bool TextualControl::IsMultiline() const noexcept {
    return text_model_->IsMultiline();
}

void TextualControl::SetIsMultiline(bool is_multiline) {
    text_model_->SetIsMultiline(is_multiline);
}


textual::LineBreak TextualControl::LineBreak() const noexcept {
    return text_model_->LineBreak();
}

void TextualControl::SetLineBreak(textual::LineBreak line_break) {
    text_model_->SetLineBreak(line_break);
}


Color TextualControl::TextColor() const {
    return text_model_->StyledText().DefaultTextColor();
}

void TextualControl::SetTextColor(const Color& color) {
    text_model_->SetTextColor(color);
}


const ColorPicker& TextualControl::TextColorPicker() const {
    return text_model_->DefaultTextColorPicker();
}

void TextualControl::SetTextColorPicker(ColorPicker picker) {
    text_model_->SetTextColorPicker(std::move(picker), *this);
}


Color TextualControl::GetTextColorAtIndex(std::size_t index) const {
    return text_model_->StyledText().GetTextColorAtIndex(index);
}


void TextualControl::SetTextColorInRange(const Color& color, const Range& range) {
    text_model_->SetTextColorInRange(color, range);
}


Color TextualControl::TextBackColor() const {
    return text_model_->StyledText().DefaultTextBackColor();
}

void TextualControl::SetTextBackColor(const Color& color) {
    text_model_->SetTextBackColor(color);
}


const ColorPicker& TextualControl::TextBackColorPicker() const {
    return text_model_->DefaultTextBackColorPicker();
}

void TextualControl::SetTextBackColorPicker(ColorPicker picker) {
    text_model_->SetTextBackColorPicker(std::move(picker), *this);
}


Color TextualControl::GetTextBackColorAtIndex(std::size_t index) const {
    return text_model_->StyledText().GetTextBackColorAtIndex(index);
}


void TextualControl::SetTextBackColorInRange(const Color& color, const Range& range) {
    text_model_->SetTextBackColorInRange(color, range);
}


const Frame& TextualControl::TextBackPadding() const {
    return text_back_padding_;
}


void TextualControl::SetTextBackPadding(const Frame& padding) {

    if (text_back_padding_ == padding) {
        return;
    }

    text_back_padding_ = padding;
    NeedRepaint();
}


const Font& TextualControl::Font() const {
    return text_model_->StyledText().DefaultFont();
}

void TextualControl::SetFont(zaf::Font font) {
    text_model_->SetFont(std::move(font));
}


const std::wstring& TextualControl::FontFamily() const {
    return Font().family_name;
}

void TextualControl::SetFontFamily(std::wstring family) {
    auto font = Font();
    font.family_name = std::move(family);
    SetFont(font);
}


float TextualControl::FontSize() const {
    return Font().size;
}

void TextualControl::SetFontSize(float size) {
    auto font = Font();
    font.size = size;
    SetFont(font);
}


FontWeight TextualControl::FontWeight() const {
    return Font().weight;
}

void TextualControl::SetFontWeight(zaf::FontWeight weight) {
    auto font = Font();
    font.weight = weight;
    SetFont(font);
}


const zaf::Font& TextualControl::GetFontAtIndex(std::size_t index) const {
    return text_model_->StyledText().GetFontAtIndex(index);
}


void TextualControl::SetFontInRange(zaf::Font font, const Range& range) {
    text_model_->SetFontInRange(std::move(font), range);
}


std::shared_ptr<textual::InlineObject> TextualControl::GetInlineObjectAtIndex(
    std::size_t index) const {

    return text_model_->StyledText().GetInlineObjectAtIndex(index);
}


void TextualControl::AttachInlineObjectToRange(
    std::shared_ptr<textual::InlineObject> inline_object,
    const Range& range) {

    ZAF_EXPECT(inline_object);
    ZAF_EXPECT(!inline_object->Host());

    text_model_->AttachInlineObjectToRange(std::move(inline_object), range);
}


void TextualControl::SetInlineObjectInRange(
    std::shared_ptr<textual::InlineObject> inline_object,
    const Range& range) {

    this->SetTextInRange(std::wstring(1, textual::ObjectReplacementChar), range);
    this->AttachInlineObjectToRange(std::move(inline_object), { range.index, 1 });
}


dwrite::TextAlignment TextualControl::TextAlignment() const {
    return text_alignment_;
}

void TextualControl::SetTextAlignment(dwrite::TextAlignment alignment) {

    text_alignment_ = alignment;

    if (text_layout_) {
        text_layout_.SetTextAlignment(alignment);
    }

    NeedRepaint();
}


dwrite::ParagraphAlignment TextualControl::ParagraphAlignment() const {
    return paragraph_alignment_;
}

void TextualControl::SetParagraphAlignment(dwrite::ParagraphAlignment alignment) {

    paragraph_alignment_ = alignment;

    if (text_layout_) {
        text_layout_.SetParagraphAlignment(alignment);
    }

    NeedRepaint();
}


dwrite::WordWrapping TextualControl::WordWrapping() const {
    return word_wrapping_;
}

void TextualControl::SetWordWrapping(dwrite::WordWrapping word_wrapping) {

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

void TextualControl::SetTextTrimming(const dwrite::TextTrimming& text_trimming) {

    text_trimming_ = text_trimming;

    if (text_layout_) {
        auto applied_text_trimming = text_trimming;
        ReviseTextTrimmingSign(applied_text_trimming, text_layout_);
        text_layout_.SetTextTrimming(applied_text_trimming);
    }

    NeedRepaint();
}


dwrite::LineSpacing TextualControl::LineSpacing() const {
    return line_spacing_;
}

void TextualControl::SetLineSpacing(const dwrite::LineSpacing& line_spacing) {

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


void TextualControl::OnInlineObjectAttached(const internal::InlineObjectAttachedInfo& event_info) {

    auto host = As<TextualControl>(shared_from_this());

    for (const auto& each_object : event_info.attached_objects) {
        each_object->SetHost(host);
    }
}


void TextualControl::OnTextModelChanged(const internal::TextModelChangedInfo& event_info) {

    //Text layout needs to be released if the text is changed or the entire text's attribute is
    //changed.
    if (event_info.IsTextChanged() || !event_info.ChangedRange().has_value()) {
        ReleaseTextLayout();
    }
    //Update the text layout if it doesn't need to be released.
    else if (text_layout_ && !event_info.IsTextColorChanged()) {

        const auto& styled_text = text_model_->StyledText();
        auto range_index = event_info.ChangedRange()->Range().index;
        Range new_range{ range_index, event_info.ChangedRange()->NewLength() };

        if (event_info.IsFontChanged()) {
            const auto& font = styled_text.GetFontAtIndex(range_index);
            SetFontToTextLayout(font, new_range, text_layout_);
        }

        if (event_info.IsInlineObjectChanged()) {
            auto object = styled_text.GetInlineObjectAtIndex(range_index);
            if (object) {
                SetInlineObjectToTextLayout(object, new_range, text_layout_);
            }
        }
    }

    if (event_info.IsTextChanged() || 
        event_info.IsFontChanged() || 
        event_info.IsInlineObjectChanged()) {

        NeedRelayout();
        RaiseContentChangedEvent();

        if (event_info.IsTextChanged()) {
            OnTextChanged(TextChangedInfo{ As<TextualControl>(shared_from_this()) });
        }
    }

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

    auto text = text_model_->Text();

    auto text_layout = GraphicFactory::Instance().CreateTextLayout(
        text,
        CreateTextFormat(),
        text_rect_.size);

    if (Font().has_underline) {
        Range range{ 0, text.length() };
        text_layout.SetHasUnderline(true, range);
    }

    for (const auto& each_item : text_model_->StyledText().RangedFonts()) {
        SetFontToTextLayout(each_item.Value(), each_item.Range(), text_layout);
    }

    for (const auto& each_item : text_model_->StyledText().InlineObjects()) {
        SetInlineObjectToTextLayout(each_item.Object(), each_item.Range(), text_layout);
    }

    return text_layout;
}


TextFormat TextualControl::CreateTextFormat() const {

    const auto& default_font = Font();

    TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = default_font.family_name;
    text_format_properties.font_size = default_font.size;
    text_format_properties.font_weight = default_font.weight;
    text_format_properties.font_style = default_font.style;

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


void TextualControl::SetInlineObjectToTextLayout(
    std::shared_ptr<textual::InlineObject> object, 
    const Range& range,
    TextLayout& text_layout) const {
    
    auto bridge = MakeCOMPtr<internal::TextInlineObjectBridge>(
        std::move(object),
        std::move(inline_object_painter_));

    text_layout.SetInlineObject(TextInlineObject{ bridge }, range);
}


void TextualControl::ReleaseTextLayout() {
    text_layout_ = {};
}

}