#pragma once

#include <string_view>
#include <zaf/base/range.h>
#include <zaf/control/control.h>
#include <zaf/control/event/text_changed_info.h>
#include <zaf/control/internal/range_map.h>
#include <zaf/graphic/font/font.h>
#include <zaf/graphic/font/font_weight.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/graphic/text/text_trimming.h>
#include <zaf/graphic/text/word_wrapping.h>

#undef max

namespace zaf {

class TextFormat;
class TextLayout;
class TextSource;

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

    void SetTextSource(std::unique_ptr<TextSource> text_source);

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
     Get text color at specified text position.

     Return TextColor() if the text color at this position has not been set.
     */
    Color GetTextColorAtPosition(std::size_t position) const {
        return GetTextColorPickerAtPosition(position)(*this);
    }

    /**
     Set color at specified text range.
     */
    void SetTextColorAtRange(const Color& color, const Range& range) {
        SetTextColorPickerAtRange(CreateColorPicker(color), range);
    }

    /**
     Get text color picker at specified text position.

     Return TextColorPicker() if the text color picker at this position has not been set.
     */
    ColorPicker GetTextColorPickerAtPosition(std::size_t position) const;

    /**
     Set color picker at specified text range.
     */
    void SetTextColorPickerAtRange(const ColorPicker& color_picker, const Range& range);

    /**
     Reset all particular text color pickers to default text color picker.

     This method removes all text color pickers that set with SetTextColorPickerAtRange.
     */
    void ResetTextColorPickers();

    /**
     Get default font.

     Return Font::Default() if this property has not been set.
     */
    virtual Font Font() const;

    /**
     Set default font.
     */
    virtual void SetFont(const zaf::Font& font);

    float FontSize() const;
    void SetFontSize(float size);

    FontWeight FontWeight() const;
    void SetFontWeight(zaf::FontWeight weight);

    /**
     Get font at specified text position.

     Return Default() if the font at this position has not been set.
     */
    virtual zaf::Font GetFontAtPosition(std::size_t position) const;

    /**
     Set font at specified text range.
     */
    virtual void SetFontAtRange(const zaf::Font& font, const Range& range);

    /**
     Reset all particular fonts to default font.

     This method removes all fonts that set with SetFontAtRange.
     */
    virtual void ResetFonts();

    /**
     Get text alignment.

     The default value is TextAlignment::Leading.
     */
    virtual TextAlignment TextAlignment() const;

    /**
     Set text alignment.
     */
    virtual void SetTextAlignment(zaf::TextAlignment alignment);

    /**
     Get paragraph alignemnt.

     The default value is ParagraphAlignment::Near;
    */
    virtual ParagraphAlignment ParagraphAlignment() const;

    /**
     Set paragraph alignment.
     */
    virtual void SetParagraphAlignment(zaf::ParagraphAlignment alignment);

    /**
     Get word wrapping.

     The default value is WordWrapping::NoWrap;
     */
    virtual WordWrapping WordWrapping() const;

    /**
     Set word wrapping.
     */
    virtual void SetWordWrapping(zaf::WordWrapping word_wrapping);

    zaf::TextTrimming TextTrimming() const;
    void SetTextTrimming(const zaf::TextTrimming& text_trimming);

    void SetTextTrimming(TextTrimmingGranularity text_trimming_granularity) {
        zaf::TextTrimming text_trimming;
        text_trimming.SetGranularity(text_trimming_granularity);
        SetTextTrimming(text_trimming);
    }

    /**
     Get text change event.

     This event is raised when the text is changed.
     */
    Observable<TextChangedInfo> TextChangedEvent() const;

protected:
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;

    void ReleaseRendererResources() override {
        ReleaseTextLayout();
    }

    void ReleaseTextLayout();

    zaf::Size CalculatePreferredContentSize(const zaf::Size& max_size) const override;

    virtual zaf::Rect GetTextRect();

    TextLayout GetTextLayout() const;

    /**
    Notifies that the text of control is changed.

    If a derived class overrides SetText() and implements its own text management, it should call 
    this method after the text is changed in order to raise text changed event.
    */
    void NotifyTextChanged();

    /**
    Handles text changed notification. This method would be called after the text of control is 
    changed.

    The default implementation raises text changed event. Derived classes should call the same 
    method of base class to raise the event if they override it.
    */
    virtual void OnTextChanged(const TextChangedInfo& event_info);

private:
    void AfterTextChanged(bool need_send_notification);

    TextLayout CreateTextLayout() const;
    TextFormat CreateTextFormat(const zaf::Font& default_font) const;
    void SetRangedFontsToTextLayout(TextLayout& text_layout) const;

    void SetTextColorsToTextLayout(TextLayout& text_layout, Renderer& renderer);

private:
    std::unique_ptr<TextSource> text_source_;
    mutable TextLayout text_layout_;

    std::optional<zaf::Font> default_font_;
    std::shared_ptr<internal::RangeMap<zaf::Font>> font_range_map_;
    ColorPicker default_text_color_picker_;
    std::shared_ptr<internal::RangeMap<ColorPicker>> text_color_picker_map_;
    zaf::TextAlignment text_alignment_{ zaf::TextAlignment::Left };
    zaf::ParagraphAlignment paragraph_alignment{ zaf::ParagraphAlignment::Top };
    zaf::WordWrapping word_wrapping_{ zaf::WordWrapping::NoWrap };
    zaf::TextTrimming text_trimming_;

    Event<TextChangedInfo> text_changed_event_;
};

}