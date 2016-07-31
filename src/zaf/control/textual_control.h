#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
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
    TextualControl();
    ~TextualControl();

    /**
     Get text.
     */
    virtual const std::wstring GetText() const;

    /**
     Set text.
     */
    virtual void SetText(const std::wstring& text);

    /**
     Get text color.
     */
    const Color GetTextColor() const {
        return GetTextColorPicker()(*this);
    }

    /**
     Get the color picker of text.
     */
    const ColorPicker GetTextColorPicker() const;

    /**
     Set text color.
     */
    void SetTextColor(const Color& color) {
        SetTextColorPicker(CreateColorPicker(color));
    }

    /**
     Set the color picker of text. 
     */
    void SetTextColorPicker(const ColorPicker& color_picker);

    /**
     Get text font.

     Return the default font if not been set.
     */
    virtual const Font GetFont() const;

    /**
     Set text font.
     */
    virtual void SetFont(const Font& font);

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

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    virtual const Rect GetTextRect() const;

    std::shared_ptr<TextFormat> CreateTextFormat() const;
    std::shared_ptr<TextLayout> CreateTextLayout(const Size& layout_size) const;
};

}