#pragma once

#include <zaf/base/range.h>
#include <zaf/control/control.h>
#include <zaf/control/event/text_changed_info.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/graphic/text/line_spacing.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_trimming.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {
namespace internal {
class InlineObjectAttachedInfo;
class TextInlineObjectBridge;
class TextInlineObjectPainter;
class TextModel;
class TextModelChangedInfo;
}

namespace textual {
class InlineObject;
class StyledText;
}

class TextFormat;
class TextLayout;

/**
Represents a control that displays text.
*/
class TextualControl : public Control {
public:
    ZAF_OBJECT;

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

    void SetStyledText(textual::StyledText styled_text);

    bool IsMultiline() const noexcept;
    void SetIsMultiline(bool is_multiline);

    /**
    Gets the default text color of the textual control in current state.
    */
    Color TextColor() const;

    /**
    Sets the default text color of the textual control and applies the color to all existing text.
    */
    void SetTextColor(const Color& color);

    const ColorPicker& TextColorPicker() const;
    void SetTextColorPicker(ColorPicker picker);

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
    Gets the default text background color of the textual control in current state.
    */
    Color TextBackColor() const;

    /**
    Sets the default text background color of the textual control and applies the color to all 
    existing text.
    */
    void SetTextBackColor(const Color& color);

    const ColorPicker& TextBackColorPicker() const;
    void SetTextBackColorPicker(ColorPicker picker);

    /**
    Gets the text background color at the specified text index in current state.
    */
    Color GetTextBackColorAtIndex(std::size_t index) const;

    /**
    Sets the specified text background color to the specified range within the existing range.

    @param color
        The text background color to set.

    @param range
        The range within the existing text where the text background color will be set.

    @throw std::logic_error
        Thrown if the range is not entirely within the existing text.
    */
    void SetTextBackColorInRange(const Color& color, const Range& range);

    /**
    Gets the padding for the text background.
    */
    const Frame& TextBackPadding() const;

    /**
    Sets the padding for the text background.

    @param padding
        The padding to be applied to the text background.

    @remark
        The padding for the text background creates additional spacing around the area of the 
        background. This can be useful when setting both the text background color and the line 
        spacing. In such cases, the text background color is painted across the entire vertical 
        spacing of the line, which may extend beyond the height of the text. By setting the
        padding, you can adjust the height of the background to better fit the text.
    */
    void SetTextBackPadding(const Frame& padding);

    /**
    Gets the default font of the textual control.
    */
    const zaf::Font& Font() const;

    /**
    Sets the default font of the textual control and applies the font to all existing text.
    */
    void SetFont(zaf::Font font);

    /**
    Gets the family name of the default font.
    */
    const std::wstring& FontFamily() const;

    /**
    Sets the family name of the default font and applies the font to all existing text.
    */
    void SetFontFamily(std::wstring family);

    /**
    Gets the size of the default font.
    */
    float FontSize() const;

    /**
    Sets the size of the default font and applies the font to all existing text.
    */
    void SetFontSize(float size);

    /**
    Gets the weight of the default font.
    */
    FontWeight FontWeight() const;

    /**
    Sets the weight of the default font and applies the font to all existing text.
    */
    void SetFontWeight(zaf::FontWeight weight);

    /**
    Gets the font at the specified text index.
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

    /**
    Gets the attached inline object at the specified text index.

    @param index
        The index of the text.

    @return 
        The inline object attached to the text at specified index, or nullptr if there is no 
        attached inline object.

    @throw std::logic_error
        Thrown if index is greater than the length of the text.
    */
    std::shared_ptr<textual::InlineObject> GetInlineObjectAtIndex(std::size_t index) const;

    /**
    Attaches the specified inline object to the specified range within the existing text.

    @param inline_object
        The inline object to attach.

    @param range
        The range within the existing text where the inline object will be attached.

    @throw std::logic_error
        Thrown if the inline_object is nullptr, or if the inline_object has been attached to a
        textual control, or if the range is not entirely within the existing text.

    @remark
        After being attached, the inline object will be displayed instead of the sub-range text. 
        Any content modification to the sub-range text will remove the attachment. If there are 
        inline objects that are already attached with the sub-range text, they will be removed as
        well.
    */
    void AttachInlineObjectToRange(
        std::shared_ptr<textual::InlineObject> inline_object,
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
     Get paragraph alignment.

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
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;

    virtual void PaintTextBack(
        Canvas& canvas, 
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout,
        const zaf::Rect& layout_rect) const;

    virtual void PaintText(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout,
        const zaf::Rect& layout_rect) const;

    void ReleaseRendererResources() override;

    zaf::Size CalculatePreferredContentSize(const zaf::Size& max_size) const override;

    virtual zaf::Rect DetermineTextRect();
    void UpdateTextRect(const zaf::Rect& text_rect);

    const internal::TextModel& TextModel() const {
        return *text_model_;
    }

    internal::TextModel& TextModel() {
        return *text_model_;
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
    void PaintTextBackInRange(
        Canvas& canvas, 
        const TextLayout& text_layout, 
        const Range& range,
        const Color& color) const;

    void OnInlineObjectAttached(const internal::InlineObjectAttachedInfo&);
    void OnTextModelChanged(const internal::TextModelChangedInfo&);

    TextLayout CreateTextLayout() const;
    TextFormat CreateTextFormat() const;
    void SetInlineObjectToTextLayout(
        std::shared_ptr<textual::InlineObject> object,
        const Range& range,
        TextLayout& text_layout) const;

    void ReleaseTextLayout();

private:
    std::unique_ptr<internal::TextModel> text_model_;
    Event<TextChangedInfo> text_changed_event_;

    std::shared_ptr<internal::TextInlineObjectPainter> inline_object_painter_;

    zaf::TextAlignment text_alignment_{ TextAlignment::Leading };
    zaf::ParagraphAlignment paragraph_alignment_{ ParagraphAlignment::Near };
    zaf::WordWrapping word_wrapping_{ WordWrapping::NoWrap };
    zaf::TextTrimming text_trimming_;
    zaf::LineSpacing line_spacing_;
    Frame text_back_padding_;
    bool ignore_tailing_white_spaces_{};

    zaf::Rect text_rect_;
    mutable TextLayout text_layout_;
};

ZAF_OBJECT_BEGIN(TextualControl);
ZAF_OBJECT_PROPERTY(IgnoreTailingWhiteSpaces)
ZAF_OBJECT_PROPERTY(IsMultiline)
ZAF_OBJECT_PROPERTY(Text)
ZAF_OBJECT_PROPERTY(TextLength)
ZAF_OBJECT_PROPERTY(TextColor)
ZAF_OBJECT_PROPERTY(Font)
ZAF_OBJECT_PROPERTY(FontFamily)
ZAF_OBJECT_PROPERTY(FontSize)
ZAF_OBJECT_PROPERTY(FontWeight)
ZAF_OBJECT_PROPERTY(TextAlignment)
ZAF_OBJECT_PROPERTY(ParagraphAlignment)
ZAF_OBJECT_PROPERTY(WordWrapping)
ZAF_OBJECT_END;

}