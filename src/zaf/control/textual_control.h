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
class TextInlineObjectBridge;
class TextInlineObjectPainter;
class TextModel;
}

class CustomTextInlineObject;
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
    Gets the length of the text in the textual control.
    */
    std::size_t TextLength() const;

    /**
    Gets the text of the textual control.
    */
    const std::wstring& Text() const;

    /**
    Sets the text of the textual control.

    @param text
        The text to set.

    @remark
        The style of the new text will be set to default.
        TextChangedEvent will be raised after setting the text.
    */
    void SetText(std::wstring text);

    /**
    Sets the specified text to the specified range within the existing text.

    @param text
        The text to set.

    @param range
        The range within the existing text where the new text will be set.

    @throw std::logic_error
        Thrown if the range is not entirely within the existing text.

    @remark
        The style of the new text will be set to default.
        TextChangedEvent will be raised after setting the text.
    */
    void SetTextInRange(std::wstring_view text, const Range& range);

    /**
    Gets the default text color of the textual control in current state.
    */
    Color TextColor() const;

    /**
    Sets the default text color of the textual control and applies the color to all existing text.
    */
    void SetTextColor(const Color& color);

    /**
    Gets the text color at the specified text index in current state.
    */
    Color GetTextColorAtIndex(std::size_t index) const;

    /**
    Sets the specified text color to the specified range within the existing range.

    @param color
        The text color to set.

    @param range
        The range within the existing text where the text color will be set.

    @throw std::logic_error
        Thrown if the range is not entirely within the existing text.
    */
    void SetTextColorInRange(const Color& color, const Range& range);

    /**
    Gets the default text color picker of the textual control.
    */
    const ColorPicker& TextColorPicker() const;

    /**
    Sets the default text color picker of the textual control and applies the color picker to all
    existing text.
    */
    void SetTextColorPicker(ColorPicker color_picker);

    /**
    Gets the text color picker at the specified text index.
    */
    const ColorPicker& GetTextColorPickerAtIndex(std::size_t index) const;

    /**
    Sets the specified color picker to the specified range within the existing text.

    @param color_picker
        The text color picker to set.

    @param range
        The range within the existing text where the text color picker will be set.

    @throw std::logic_error
        Thrown if the range is not entirely within the existing text.
    */
    void SetTextColorPickerInRange(ColorPicker color_picker, const Range& range);

    /**
    Gets the default font of the textual control.
    */
    const zaf::Font& Font() const;

    /**
    Sets the default font of the textual control and applies the font to all existing text.
    */
    void SetFont(zaf::Font font);

    const std::wstring& FontFamily() const;
    void SetFontFamily(std::wstring family);

    float FontSize() const;
    void SetFontSize(float size);

    FontWeight FontWeight() const;
    void SetFontWeight(zaf::FontWeight weight);

    /**
    Gets the font at the specified text index.

    Returns Default() if the font at the specified index is not set.
    */
    const zaf::Font& GetFontAtIndex(std::size_t index) const;

    /**
    Sets the specified font to the specified range within the existing text.

    @param font
        The font to set.

    @param range
        The range within the existing text where the font will be set.

    @throw std::logic_error
        Thrown if the range is not entirely within the existing text.
    */
    void SetFontInRange(zaf::Font font, const Range& range);

    void SetInlineObjectInRange(
        std::shared_ptr<CustomTextInlineObject> inline_object,
        const Range& range);

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
    void UpdateTextRect(const zaf::Rect& text_rect);

    const internal::TextModel& TextModel() const {
        return text_model_;
    }

    internal::TextModel& TextModel() {
        return text_model_;
    }

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

    void OnTextModelChanged(const internal::TextModelChangedInfo&);

    TextLayout CreateTextLayout() const;
    TextFormat CreateTextFormat() const;

    void ReleaseTextLayout();

private:
    internal::TextModel text_model_;
    Event<TextChangedInfo> text_changed_event_;

    std::shared_ptr<internal::TextInlineObjectPainter> inline_object_painter_;

    zaf::TextAlignment text_alignment_{ TextAlignment::Leading };
    zaf::ParagraphAlignment paragraph_alignment_{ ParagraphAlignment::Near };
    zaf::WordWrapping word_wrapping_{ WordWrapping::NoWrap };
    zaf::TextTrimming text_trimming_;
    zaf::LineSpacing line_spacing_;
    bool ignore_tailing_white_spaces_{};

    zaf::Rect text_rect_;
    mutable TextLayout text_layout_;
};

}