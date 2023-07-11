#pragma once

#include <variant>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/graphic/canvas.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_trimming.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf::internal {

class SetTextResult {
public:
    bool is_changed{};
    bool is_notification_sent{};
};


class TextualControlCore : NonCopyableNonMovable {
public:
    TextualControlCore() = default;
    virtual ~TextualControlCore() = default;

    virtual void Initialize(const std::shared_ptr<TextualControl>& owner) = 0;

    virtual std::size_t GetTextLength() = 0;
    virtual std::variant<std::wstring_view, std::wstring> GetText() = 0;
    virtual SetTextResult SetText(const std::wstring& text) = 0;

    virtual Font GetFont() = 0;
    virtual void SetFont(const Font& font) = 0;
    virtual Font GetFontAtPosition(std::size_t position) = 0;
    virtual void SetFontAtRange(const zaf::Font& font, const Range& range) = 0;
    virtual void ResetFonts() = 0;

    virtual ColorPicker GetTextColorPicker() = 0;
    virtual void SetTextColorPicker(const ColorPicker& color_picker) = 0;
    virtual ColorPicker GetTextColorPickerAtPosition(std::size_t position) = 0;
    virtual void SetTextColorPickerAtRange(
        const ColorPicker& color_picker, 
        const Range& range) = 0;
    virtual void ResetTextColorPickers() = 0;

    virtual TextAlignment GetTextAlignment() = 0;
    virtual void SetTextAlignment(TextAlignment alignment) = 0;

    virtual ParagraphAlignment GetParagraphAlignment() = 0;
    virtual void SetParagraphAlignment(ParagraphAlignment alignment) = 0;

    virtual WordWrapping GetWordWrapping() = 0;
    virtual void SetWordWrapping(WordWrapping word_wrapping) = 0;

    virtual TextTrimming GetTextTrimming() = 0;
    virtual void SetTextTrimming(const TextTrimming& text_trimming) = 0;

    virtual Size CalculateTextSize(const Size& boundary_size) = 0;
    virtual void LayoutText(const Size& text_size) = 0;
    virtual void PaintText(Canvas& canvas, const Rect& dirty_rect, const Rect& text_rect) = 0;
    virtual void ReleaseRendererResources() = 0;
};

}