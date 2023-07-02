#pragma once

#include <functional>
#include <optional>
#include <zaf/control/internal/range_map.h>
#include <zaf/control/internal/textual_control/textual_control_core.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf::internal {

class GeneralTextualCore : public TextualControlCore {
public:
    void Initialize(const std::shared_ptr<TextualControl>& owner) override;

    std::size_t GetTextLength() override;
    std::variant<std::wstring_view, std::wstring> GetText() override;
    SetTextResult SetText(const std::wstring& text) override;

    Font GetFont() override {
        return default_font_;
    }
    void SetFont(const Font& font) override;
    virtual Font GetFontAtPosition(std::size_t position) override;
    virtual void SetFontAtRange(const zaf::Font& font, const Range& range) override;
    virtual void ResetFonts() override;

    ColorPicker GetTextColorPicker() override;
    void SetTextColorPicker(const ColorPicker& color_picker) override;
    ColorPicker GetTextColorPickerAtPosition(std::size_t position) override;
    void SetTextColorPickerAtRange(const ColorPicker& color_picker, const Range& range) override;
    void ResetTextColorPickers() override;

    TextAlignment GetTextAlignment() override {
        return text_alignment_;
    }
    void SetTextAlignment(TextAlignment alignment) override;

    ParagraphAlignment GetParagraphAlignment() override {
        return paragraph_alignment_;
    }
    void SetParagraphAlignment(ParagraphAlignment alignment) override;

    WordWrapping GetWordWrapping() override {
        return word_wrapping_;
    }
    void SetWordWrapping(WordWrapping word_wrapping) override;

    TextTrimming GetTextTrimming() override {
        return text_trimming_;
    }
    void SetTextTrimming(const zaf::TextTrimming& text_trimming) override;

    void PaintText(Canvas& canvas, const Rect& dirty_rect, const Rect& text_rect) override;
    void ReleaseRendererResources() override;

    TextLayout GetTextLayout();

private:
    TextLayout CreateTextLayout() const;
    TextFormat CreateTextFormat() const;
    void SetRangeFontsToTextLayout(TextLayout& text_layout) const;
    void SetTextColorsToTextLayout(
        TextLayout& text_layout, 
        Renderer& renderer,
        const TextualControl& textual_control) const;
    void ReleaseTextLayout();
    void NotifyRepaint();

private:
    std::weak_ptr<TextualControl> owner_;
    std::wstring text_;

    zaf::Font default_font_{ Font::Default() };
    std::unique_ptr<internal::RangeMap<Font>> font_range_map_;

    ColorPicker default_text_color_picker_;
    std::unique_ptr<internal::RangeMap<ColorPicker>> text_color_picker_map_;

    TextAlignment text_alignment_{ TextAlignment::Leading };
    ParagraphAlignment paragraph_alignment_{ ParagraphAlignment::Near };
    WordWrapping word_wrapping_{ WordWrapping::NoWrap };
    TextTrimming text_trimming_;

    TextLayout text_layout_;
};

}