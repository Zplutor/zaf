#pragma once

#include <zaf/base/range.h>
#include <zaf/control/control.h>
#include <zaf/control/event/text_changed_info.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/graphic/text/line_spacing.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_trimming.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {
namespace internal {
class TextModel;
}

class TextFormat;
class TextLayout;

/**
 Represents a textual control.

 This is the base class of all controls that can display text.
 */
class TextualControl : public Control {
public:
    ZAF_DECLARE_TYPE

public:
    TextualControl();
    ~TextualControl();

    /**
     Get text length.
     */
    std::size_t TextLength() const;

    /**
     Get text.
     */
    std::wstring Text() const;

    /**
     Set text.
     */
    void SetText(const std::wstring& text);

    /**
     Get the default text color.
     */
    Color TextColor() const {
        return TextColorPicker()(*this);
    }

    /**
     Get the default color picker of text.
     */
    ColorPicker TextColorPicker() const;

    /**
     Set the default text color.
     */
    void SetTextColor(const Color& color) {
        SetTextColorPicker(CreateColorPicker(color));
    }

    /**
     Set the default color picker of text. 
     */
    void SetTextColorPicker(const ColorPicker& color_picker);

    /**
    Gets the text color at the specified text index.

    Returns the default text color if the text color at the specified index is not set.
    */
    Color GetTextColorAtIndex(std::size_t index) const {
        return GetTextColorPickerAtIndex(index)(*this);
    }

    /**
    Set text color in the specified text range.
    */
    void SetTextColorInRange(const Color& color, const Range& range) {
        SetTextColorPickerInRange(CreateColorPicker(color), range);
    }

    /**
    Gets the text color picker at the specified text index.

    Returns the default text color picker if the text color picker at the specified index is not 
    set.
    */
    ColorPicker GetTextColorPickerAtIndex(std::size_t index) const;

    /**
    Sets color picker in the specified text range.
    */
    void SetTextColorPickerInRange(const ColorPicker& color_picker, const Range& range);

    /**
     Reset all particular text color pickers to default text color picker.

     This method removes all text color pickers that set with SetTextColorPickerInRange.
     */
    void ResetTextColorPickers();

    /**
     Get default font.

     Return Font::Default() if this property has not been set.
     */
    zaf::Font Font() const;

    /**
     Set default font.
     */
    void SetFont(const zaf::Font& font);

    std::wstring FontFamily() const;
    void SetFontFamily(const std::wstring& family);

    float FontSize() const;
    void SetFontSize(float size);

    FontWeight FontWeight() const;
    void SetFontWeight(zaf::FontWeight weight);

    /**
    Gets the font at the specified text index.

    Returns Default() if the font at the specified index is not set.
    */
    zaf::Font GetFontAtIndex(std::size_t index) const;

    /**
    Sets font in the specified text range.
    */
    void SetFontInRange(const zaf::Font& font, const Range& range);

    /**
     Reset all particular fonts to default font.

     This method removes all fonts that set with SetFontInRange.
     */
    void ResetFonts();

    /**
     Get text alignment.

     The default value is TextAlignment::Leading.
     */
    TextAlignment TextAlignment() const;

    /**
     Set text alignment.
     */
    void SetTextAlignment(zaf::TextAlignment alignment);

    /**
     Get paragraph alignemnt.

     The default value is ParagraphAlignment::Near;
    */
    ParagraphAlignment ParagraphAlignment() const;

    /**
     Set paragraph alignment.
     */
    void SetParagraphAlignment(zaf::ParagraphAlignment alignment);

    /**
     Get word wrapping.

     The default value is WordWrapping::NoWrap;
     */
    WordWrapping WordWrapping() const;

    /**
     Set word wrapping.
     */
    void SetWordWrapping(zaf::WordWrapping word_wrapping);

    zaf::TextTrimming TextTrimming() const;
    void SetTextTrimming(const zaf::TextTrimming& text_trimming);

    void SetTextTrimming(TextTrimmingGranularity text_trimming_granularity) {
        zaf::TextTrimming text_trimming;
        text_trimming.SetGranularity(text_trimming_granularity);
        SetTextTrimming(text_trimming);
    }

    zaf::LineSpacing LineSpacing() const;
    void SetLineSpacing(const zaf::LineSpacing& line_spacing);

    bool IgnoreTailingWhiteSpaces() const;
    void SetIgnoreTailingWhiteSpaces(bool value);

    /**
     Get text change event.

     This event is raised when the text is changed.
     */
    Observable<TextChangedInfo> TextChangedEvent() const;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;

    virtual void PaintTextLayout(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout,
        const zaf::Rect& layout_rect);

    void ReleaseRendererResources() override;

    zaf::Size CalculatePreferredContentSize(const zaf::Size& max_size) const override;

    virtual zaf::Rect DetermineTextRect();

    TextLayout GetTextLayout() const;

    /**
    Handles text changed notification. This method would be called after the text of control is 
    changed.

    The default implementation raises text changed event. Derived classes should call the same 
    method of base class to raise the event if they override it.
    */
    virtual void OnTextChanged(const TextChangedInfo& event_info);

private:
    void SetTextColorsToTextLayout(TextLayout& text_layout, Renderer& renderer) const;

    void InnerSetFont(const zaf::Font& new_font);
    void OnTextModelChanged(const internal::TextModelChangedInfo&);

    TextLayout CreateTextLayout() const;
    TextFormat CreateTextFormat() const;

    void ReleaseTextLayout();

private:
    std::unique_ptr<internal::TextModel> text_model_;
    Event<TextChangedInfo> text_changed_event_;

    zaf::Font default_font_{ zaf::Font::Default() };
    internal::RangeMap<zaf::Font> ranged_font_;

    ColorPicker default_text_color_picker_;
    internal::RangeMap<ColorPicker> ranged_text_color_picker_;

    zaf::TextAlignment text_alignment_{ TextAlignment::Leading };
    zaf::ParagraphAlignment paragraph_alignment_{ ParagraphAlignment::Near };
    zaf::WordWrapping word_wrapping_{ WordWrapping::NoWrap };
    zaf::TextTrimming text_trimming_;
    zaf::LineSpacing line_spacing_;
    bool ignore_tailing_white_spaces_{};

    zaf::Rect layout_rect_;
    mutable TextLayout text_layout_;
};

}