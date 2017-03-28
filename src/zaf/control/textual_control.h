#pragma once

#include <zaf/base/event.h>
#include <zaf/control/control.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/graphic/text/text_range.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

class Font;
class TextFormat;
class TextLayout;

/**
 Represents a textual control.

 This is the base class of all controls that can display text.
 */
class TextualControl : public Control {
public:
    /**
     Type of text change event.
     */
    typedef Event<const std::shared_ptr<TextualControl>&> TextChangeEvent;

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
    virtual const std::wstring GetText() const;

    /**
     Set text.
     */
    virtual void SetText(const std::wstring& text);

    /**
     Get the default text color.
     */
    const Color GetDefaultTextColor() const {
        return GetDefaultTextColorPicker()(*this);
    }

    /**
     Get the default color picker of text.
     */
    const ColorPicker GetDefaultTextColorPicker() const;

    /**
     Set the default text color.
     */
    void SetDefaultTextColor(const Color& color) {
        SetDefaultTextColorPicker(CreateColorPicker(color));
    }

    /**
     Set the default color picker of text. 
     */
    void SetDefaultTextColorPicker(const ColorPicker& color_picker);

    /**
     Get text color at specified text position.

     Return GetDefaultTextColor() if the text color at this position has not been set.
     */
    const Color GetTextColorAtPosition(std::size_t position) const {
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

     Return GetDefaultTextColorPicker() if the text color picker at this position has not been set.
     */
    const ColorPicker GetTextColorPickerAtPosition(std::size_t position) const;

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
    virtual const Font GetDefaultFont() const;

    /**
     Set default font.
     */
    virtual void SetDefaultFont(const Font& font);

    /**
     Get font at specified text position.

     Return GetDefault() if the font at this position has not been set.
     */
    virtual const Font GetFontAtPosition(std::size_t position) const;

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

    /**
     Get text change event.

     This event is raised when the text is changed.
     */
    TextChangeEvent::Proxy GetTextChangeEvent();

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;

    void ReleaseRendererResources() override {
        ReleaseTextLayout();
    }

    virtual const Rect GetTextRect() const;

    TextLayout GetTextLayout() {
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
    void NotifyTextChange();

private:
    const TextLayout CreateTextLayout();
    const TextFormat CreateTextFormat() const;
    void SetFontsToTextLayout(TextLayout& text_layout);

    void SetTextColorsToTextLayout(TextLayout& text_layout, Renderer& renderer);

    void ReleaseTextLayout();

private:
    TextLayout text_layout_;
};

}