#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/graphic/text/text_range.h>
#include <zaf/graphic/text/text_trimming.h>
#include <zaf/graphic/text/word_wrapping.h>

#undef max

namespace zaf {

class Font;
class TextFormat;
class TextLayout;
class TextualControlTextChangeInfo;

/**
 Represents a textual control.

 This is the base class of all controls that can display text.
 */
class TextualControl : public Control {
public:
    ZAF_DECLARE_REFLECTION_TYPE

public:
    TextualControl();
    ~TextualControl();

    /**
     Get text length.
     */
    std::size_t GetTextLength() const {
        return GetText().length();
    }

    /**
     Get text.
     */
    virtual std::wstring GetText() const;

    /**
     Set text.
     */
    virtual void SetText(const std::wstring& text);

    /**
     Get the default text color.
     */
    Color GetTextColor() const {
        return GetTextColorPicker()(*this);
    }

    /**
     Get the default color picker of text.
     */
    ColorPicker GetTextColorPicker() const;

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

     Return GetTextColor() if the text color at this position has not been set.
     */
    Color GetTextColorAtPosition(std::size_t position) const {
        return GetTextColorPickerAtPosition(position)(*this);
    }

    /**
     Set color at specified text range.
     */
    void SetTextColorAtRange(const Color& color, const TextRange& range) {
        SetTextColorPickerAtRange(CreateColorPicker(color), range);
    }

    /**
     Get text color picker at specified text position.

     Return GetTextColorPicker() if the text color picker at this position has not been set.
     */
    ColorPicker GetTextColorPickerAtPosition(std::size_t position) const;

    /**
     Set color picker at specified text range.
     */
    void SetTextColorPickerAtRange(const ColorPicker& color_picker, const TextRange& range);

    /**
     Reset all particular text color pickers to default text color picker.

     This method removes all text color pickers that set with SetTextColorPickerAtRange.
     */
    void ResetTextColorPickers();

    /**
     Get default font.

     Return Font::GetDefault() if this property has not been set.
     */
    virtual Font GetFont() const;

    /**
     Set default font.
     */
    virtual void SetFont(const Font& font);

    float GetFontSize() const;
    void SetFontSize(float size);

    int GetFontWeight() const;
    void SetFontWeight(int weight);

    /**
     Get font at specified text position.

     Return GetDefault() if the font at this position has not been set.
     */
    virtual Font GetFontAtPosition(std::size_t position) const;

    /**
     Set font at specified text range.
     */
    virtual void SetFontAtRange(const Font& font, const TextRange& range);

    /**
     Reset all particular fonts to default font.

     This method removes all fonts that set with SetFontAtRange.
     */
    virtual void ResetFonts();

    /**
     Get text alignment.

     The default value is TextAlignment::Leading.
     */
    virtual TextAlignment GetTextAlignment() const;

    /**
     Set text alignment.
     */
    virtual void SetTextAlignment(TextAlignment alignment);

    /**
     Get paragraph alignemnt.

     The default value is ParagraphAlignment::Near;
    */
    virtual ParagraphAlignment GetParagraphAlignment() const;

    /**
     Set paragraph alignment.
     */
    virtual void SetParagraphAlignment(ParagraphAlignment alignment);

    /**
     Get word wrapping.

     The default value is WordWrapping::NoWrap;
     */
    virtual WordWrapping GetWordWrapping() const;

    /**
     Set word wrapping.
     */
    virtual void SetWordWrapping(WordWrapping word_wrapping);

    TextTrimming GetTextTrimming() const;
    void SetTextTrimming(const TextTrimming& text_trimming);

    void SetTextTrimming(TextTrimming::Granularity text_trimming_granularity) {
        TextTrimming text_trimming;
        text_trimming.granularity = text_trimming_granularity;
        SetTextTrimming(text_trimming);
    }

    /**
     Get text change event.

     This event is raised when the text is changed.
     */
    Observable<TextualControlTextChangeInfo> TextChangeEvent();

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;

    void ReleaseRendererResources() override {
        ReleaseTextLayout();
    }

    Size GetPreferredContentSize() const override;

    virtual Rect GetTextRect();

    TextLayout GetTextLayout() const {
        if (text_layout_ == nullptr) {
            text_layout_ = CreateTextLayout();
        }
        return text_layout_;
    }

    /**
     Raise the text change event.

     If a derived class overrides SetText method and implements its own text management, 
     it should call this method after the text changed in order to raise text change event.
     */
    void RaiseTextChangedEvent();

    virtual void OnTextChanged() { }

private:
    Size CalculatePreferredSize(const Size& max_size) const;
    TextLayout CreateTextLayout() const;
    TextFormat CreateTextFormat(const Font& default_font) const;
    void SetRangedFontsToTextLayout(TextLayout& text_layout) const;

    void SetTextColorsToTextLayout(TextLayout& text_layout, Renderer& renderer);

    void ReleaseTextLayout();

private:
    mutable TextLayout text_layout_;
};


class TextualControlTextChangeInfo {
public:
    std::shared_ptr<TextualControl> textual_control;
};

}