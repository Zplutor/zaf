#pragma once

#include <Richedit.h>
#include <TextServ.h>
#include <zaf/control/internal/textual_control/textual_control_core.h>
#include <zaf/control/rich_edit.h>

namespace zaf::rich_edit::internal {

class RichEditCore : public zaf::internal::TextualControlCore {
public:
    void Initialize(const std::shared_ptr<TextualControl>& owner) override;

    std::size_t GetTextLength() override;
    std::variant<std::wstring_view, std::wstring> GetText() override;
    zaf::internal::SetTextResult SetText(const std::wstring& text) override;

    Font GetFont() override;
    void SetFont(const Font& font) override;
    virtual Font GetFontAtIndex(std::size_t position) override;
    virtual void SetFontInRange(const zaf::Font& font, const Range& range) override;
    virtual void ResetFonts() override;

    ColorPicker GetTextColorPicker() override;
    void SetTextColorPicker(const ColorPicker& color_picker) override;
    ColorPicker GetTextColorPickerAtIndex(std::size_t position) override;
    void SetTextColorPickerInRange(const ColorPicker& color_picker, const Range& range) override;
    void ResetTextColorPickers() override;

    TextAlignment GetTextAlignment() override;
    void SetTextAlignment(TextAlignment alignment) override;

    ParagraphAlignment GetParagraphAlignment() override;
    void SetParagraphAlignment(ParagraphAlignment alignment) override;

    WordWrapping GetWordWrapping() override;
    void SetWordWrapping(WordWrapping word_wrapping) override;

    TextTrimming GetTextTrimming() override;
    void SetTextTrimming(const zaf::TextTrimming& text_trimming) override;

    LineSpacing GetLineSpacing() override;
    void SetLineSpacing(const LineSpacing& line_spacing) override;

    bool IgnoreTailingWhiteSpaces() const override;
    void SetIgnoreTailingWhiteSpaces(bool value) override;

    Size CalculateTextSize(const Size& boundary_size) override;
    void LayoutText(const Rect& layout_rect) override;
    void PaintText(Canvas& canvas, const Rect& dirty_rect) override;
    void ReleaseRendererResources() override;

private:
    std::weak_ptr<RichEdit> owner_;

    zaf::ParagraphAlignment paragraph_alignment_{ zaf::ParagraphAlignment::Near };
    ColorPicker text_color_picker_;
};

}