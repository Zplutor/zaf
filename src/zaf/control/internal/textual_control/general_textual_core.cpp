#include <zaf/control/internal/textual_control/general_textual_core.h>
#include <zaf/control/control.h>
#include <zaf/graphic/graphic_factory.h>
#include <zaf/graphic/text/text_format_properties.h>
#include <zaf/internal/theme.h>

namespace zaf::internal {
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
    text_layout.SetHasUnderline(font.has_underline, range);
}

}


void GeneralTextualCore::Initialize(const std::shared_ptr<TextualControl>& owner) {
    owner_ = owner;
}


std::size_t GeneralTextualCore::GetTextLength() {
    return text_.length();
}


std::variant<std::wstring_view, std::wstring> GeneralTextualCore::GetText() {
    return std::wstring_view{ text_ };
}


SetTextResult GeneralTextualCore::SetText(const std::wstring& text) {

    SetTextResult result;
    if (text_ != text) {

        text_ = text; 
        ReleaseTextLayout();

        result.is_changed = true;
    }
    return result;
}


void GeneralTextualCore::SetFont(const Font& font) {

    default_font_ = font;

    ReleaseTextLayout();
    NotifyRepaint();
}


Font GeneralTextualCore::GetFontAtPosition(std::size_t position) {

    if (font_range_map_) {

        bool is_existent = false;
        auto font = font_range_map_->GetValueAtPosition(position, &is_existent);

        if (is_existent) {
            return font;
        }
    }

    return Font();
}


void GeneralTextualCore::SetFontAtRange(const zaf::Font& font, const Range& range) {

    if (!font_range_map_) {
        font_range_map_ = std::make_unique<internal::RangeMap<zaf::Font>>();
    }

    font_range_map_->AddValueToRange(font, range.index, range.length);

    if (text_layout_) {
        SetFontToTextLayout(font, range, text_layout_);
    }

    NotifyRepaint();
}


void GeneralTextualCore::ResetFonts() {

    font_range_map_.reset();

    ReleaseTextLayout();
    NotifyRepaint();
}


ColorPicker GeneralTextualCore::GetTextColorPicker() {

    if (default_text_color_picker_) {
        return default_text_color_picker_;
    }

    return [](const Control& control) {
        if (control.IsEnabled()) {
            return Color::FromRGB(internal::ControlNormalTextColorRGB);
        }
        else {
            return Color::FromRGB(internal::ControlDisabledTextColorRGB);
        }
    };
}


void GeneralTextualCore::SetTextColorPicker(const ColorPicker& color_picker) {

    default_text_color_picker_ = color_picker;
    ReleaseTextLayout();
    NotifyRepaint();
}


ColorPicker GeneralTextualCore::GetTextColorPickerAtPosition(std::size_t position) {

    if (text_color_picker_map_) {

        auto color_picker = text_color_picker_map_->GetValueAtPosition(position);
        if (color_picker != nullptr) {
            return color_picker;
        }
    }

    return GetTextColorPicker();
}


void GeneralTextualCore::SetTextColorPickerAtRange(
    const ColorPicker& color_picker,
    const Range& range) {

    if (!text_color_picker_map_) {
        text_color_picker_map_ = std::make_unique<internal::RangeMap<ColorPicker>>();
    }

    text_color_picker_map_->AddValueToRange(color_picker, range.index, range.length);

    ReleaseTextLayout();
    NotifyRepaint();
}


void GeneralTextualCore::ResetTextColorPickers() {

    text_color_picker_map_.reset();

    ReleaseTextLayout();
    NotifyRepaint();
}


void GeneralTextualCore::SetTextAlignment(TextAlignment alignment) {

    text_alignment_ = alignment;

    if (text_layout_) {
        text_layout_.SetTextAlignment(alignment);
    }

    NotifyRepaint();
}


void GeneralTextualCore::SetParagraphAlignment(ParagraphAlignment alignment) {

    paragraph_alignment_ = alignment;

    if (text_layout_) {
        text_layout_.SetParagraphAlignment(alignment);
    }

    NotifyRepaint();
}


void GeneralTextualCore::SetWordWrapping(WordWrapping word_wrapping) {

    word_wrapping_ = word_wrapping;

    if (text_layout_) {
        text_layout_.SetWordWrapping(word_wrapping);
    }

    NotifyRepaint();
}


void GeneralTextualCore::SetTextTrimming(const zaf::TextTrimming& text_trimming) {

    text_trimming_ = text_trimming;

    if (text_layout_) {
        auto applied_text_trimming = text_trimming;
        ReviseTextTrimmingSign(applied_text_trimming, text_layout_);
        text_layout_.SetTextTrimming(applied_text_trimming);
    }

    NotifyRepaint();
}


void GeneralTextualCore::SetLineSpacing(const LineSpacing& line_spacing) {

    line_spacing_ = line_spacing;

    if (text_layout_) {
        text_layout_.SetLineSpacing(line_spacing);
    }

    NotifyRepaint();
}


Size GeneralTextualCore::CalculateTextSize(const Size& boundary_size) {

    auto text_layout = GetTextLayout();

    //Set layout size to boundary size for calculation.
    text_layout.SetMaxWidth(boundary_size.width);
    text_layout.SetMaxHeight(boundary_size.height);

    auto metrics = text_layout.GetMetrics();

    //Note that if TextAlignment is set to Center, WidthIncludingTrailingWhitespace would be 0.
    auto width =
        metrics.WidthIncludingTrailingWhitespace() != 0 ?
        metrics.WidthIncludingTrailingWhitespace() :
        metrics.Width();

    //Recover layout size.
    text_layout.SetMaxWidth(layout_rect_.size.width);
    text_layout.SetMaxHeight(layout_rect_.size.height);

    return zaf::Size{ width, metrics.Height() };
}


void GeneralTextualCore::LayoutText(const Rect& layout_rect) {

    layout_rect_ = layout_rect;

    if (text_layout_) {
        text_layout_.SetMaxWidth(layout_rect_.size.width);
        text_layout_.SetMaxHeight(layout_rect_.size.height);
    }
}


void GeneralTextualCore::PaintText(Canvas& canvas, const Rect& dirty_rect) {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    //No need to paint if layout rect is empty.
    if (layout_rect_.size.width <= 0 || layout_rect_.size.height <= 0) {
        return;
    }

    auto content_rect = owner->ContentRect();

    auto layout_rect_in_control = layout_rect_;
    layout_rect_in_control.AddOffset(content_rect.position);

    auto text_boundary = Rect::Intersect(layout_rect_in_control, content_rect);
    auto clipping_guard = canvas.PushClipping(text_boundary);

    auto text_layout = GetTextLayout();
    SetTextColorsToTextLayout(text_layout, canvas.Renderer(), *owner);

    PaintTextLayout(canvas, dirty_rect, text_layout, layout_rect_in_control);
}


void GeneralTextualCore::PaintTextLayout(
    Canvas& canvas,
    const Rect& dirty_rect,
    const TextLayout& text_layout,
    const Rect& layout_rect) {

    auto state_guard = canvas.PushState();
    canvas.SetBrushWithColor(GetTextColorPicker()(*owner_.lock()));
    canvas.DrawTextLayout(text_layout, layout_rect.position);
}


void GeneralTextualCore::SetTextColorsToTextLayout(
    TextLayout& text_layout, 
    Renderer& renderer,
    const TextualControl& textual_control) const {

    if (!text_color_picker_map_) {
        return;
    }

    auto ranges_and_text_color_pickers = text_color_picker_map_->GetAllRangesAndValues();
    for (const auto& each_pair : ranges_and_text_color_pickers) {

        auto brush = renderer.CreateSolidColorBrush(each_pair.second(textual_control));
        text_layout.SetBrush(brush, Range(each_pair.first.first, each_pair.first.second));
    }
}


void GeneralTextualCore::ReleaseRendererResources() {
    ReleaseTextLayout();
}


TextLayout GeneralTextualCore::GetTextLayout() {

    if (!text_layout_) {
        text_layout_ = CreateTextLayout();
    }

    return text_layout_;
}


TextLayout GeneralTextualCore::CreateTextLayout() const {

    auto text_layout = GraphicFactory::Instance().CreateTextLayout(
        text_, 
        CreateTextFormat(),
        layout_rect_.size);

    if (default_font_.has_underline) {
        Range range{ 0, text_.length() };
        text_layout.SetHasUnderline(true, range);
    }

    SetRangeFontsToTextLayout(text_layout);
    return text_layout;
}


TextFormat GeneralTextualCore::CreateTextFormat() const {

    TextFormatProperties text_format_properties;
    text_format_properties.font_family_name = default_font_.family_name;
    text_format_properties.font_size = default_font_.size;
    text_format_properties.font_weight = default_font_.weight;

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


void GeneralTextualCore::SetRangeFontsToTextLayout(TextLayout& text_layout) const {

    if (!font_range_map_) {
        return;
    }

    auto ranges_and_fonts = font_range_map_->GetAllRangesAndValues();
    for (const auto& each_pair : ranges_and_fonts) {

        Range range{ each_pair.first.first, each_pair.first.second };
        SetFontToTextLayout(each_pair.second, range, text_layout);
    }
}


void GeneralTextualCore::ReleaseTextLayout() {
    text_layout_ = {};
}


void GeneralTextualCore::NotifyRepaint() {

    auto owner = owner_.lock();
    if (owner) {
        owner->NeedRepaint();
    }
}

}