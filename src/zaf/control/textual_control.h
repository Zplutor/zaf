#pragma once

#include <zaf/control/control.h>
#include <zaf/graphic/text/paragraph_alignment.h>
#include <zaf/graphic/text/text_alignment.h>
#include <zaf/graphic/text/word_wrapping.h>

namespace zaf {

class Font;
class TextFormat;
class TextLayout;

class TextualControl : public Control {
public:
    TextualControl();
    ~TextualControl();

    /**
     Get the control's text value.
     */
    virtual const std::wstring GetText() const;

    /**
     Set the control's text value.
     */
    virtual void SetText(const std::wstring& text);

    const Color GetTextColor() const {
        return GetTextColorPicker()(*this);
    }

    const ColorPicker GetTextColorPicker() const;
    void SetTextColorPicker(const ColorPicker& color_picker);

    virtual const Font GetFont() const;
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

    virtual WordWrapping GetWordWrapping() const;
    virtual void SetWordWrapping(WordWrapping word_wrapping);

protected:
    void Paint(Canvas& canvas, const Rect& dirty_rect) override;
    virtual const Rect GetTextRect() const;

    std::shared_ptr<TextFormat> CreateTextFormat() const;
    std::shared_ptr<TextLayout> CreateTextLayout(const Size& layout_size) const;
};

}