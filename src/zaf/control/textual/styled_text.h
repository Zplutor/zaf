#pragma once

/**
@file
    Defines the zaf::textual::StyledText class.
*/

#include <string>
#include <zaf/base/error/contract_error.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/default_text_style.h>
#include <zaf/control/textual/inline_object.h>
#include <zaf/control/textual/ranged_text_style.h>
#include <zaf/control/textual/text_style.h>
#include <zaf/graphic/font/font.h>
#include <zaf/xml/xml_serializable.h>

namespace zaf::textual {

/**
Represents a text with various styles, such as the font, text color, and attached inline objects.

@details
    A StyledText has two kinds of styles:
    - Ranged styles, which apply to a specific sub-range of the text. Ranged styles are bound to
      the text in the sub-range; adding or removing text in the sub-range will also modify the
      range of the styles.
    - Default styles, which apply to all text without ranged styles.

    Copying a StyledText is a high-cost operation, so copy construction and copy assignment are 
    forbidden. Users can use the Clone() method to copy the StyledText explicitly if needed.
*/
class StyledText : public XMLSerializable, NonCopyable {
public:
    /**
    A class used to access the ranged fonts in a StyledText.
    */
    using RangedFontAccessor = RangedTextStyle::FontAccessor;

    /**
    A class used to access the ranged colors in a StyledText.
    */
    using RangedColorAccessor = RangedTextStyle::ColorAccessor;

public:
    /**
    Constructs an empty StyledText.
    */
    StyledText();

    /**
    Constructs a StyledText with the specified text.

    @param text
        The text that the StyledText will own.
    */
    explicit StyledText(std::wstring text);

    std::size_t Length() const noexcept {
        return text_.length();
    }

    /**
    Gets the text of the StyledText.

    @return
        The text of the StyledText.
    */
    const std::wstring& Text() const noexcept {
        return text_;
    }

    /**
    Sets the specified text to the StyledText.

    @param text
        The text to set.

    @details
        Setting a text will remove all ranged styles from the StyledText, even if the new text is
        equal to the existing text.

    @throw ... 
        Any exception thrown by the handling of inline object detaching.
    */
    void SetText(std::wstring text);

    /**
    Sets the specified text to the specified range within the existing text.

    @param text
        The text to set.

    @param range
        The range within the existing text where the new text will be set. The end index of the 
        range my exceed the length of the existing text; in such a case, the end index will be 
        revised to the length of the existing text.

    @pre 
        The start index of the range does not exceed the bounds of the existing txt.

    @return 
        The range of the new text within the whole text after setting.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if memory allocation fails.

    @throw ...
        Any exception thrown by the handling of inline object detaching.

    @details
        If the length of the range is 0, the new text will be inserted into to the existing text; 
        otherwise, the existing text in the range will be replaced with the new text.

        After setting the new text, ranged styles that are within the bounds of the range will be 
        removed.
    */
    Range SetTextInRange(std::wstring_view text, const Range& range);

    /**
    Appends the specified text to the end of the existing text.

    @param text
        The text to append.

    @return 
        The range of the new text within the whole text after appending.

    @throw std::bad_alloc
        Thrown if memory allocation fails.
    */
    Range AppendText(std::wstring_view text);

    /**
    Appends the specified text to the end of the existing text, and applies the specified text
    style to the new text.

    @param text
        The text to append.

    @param style
        The text style to apply to the new text.

    @return
        The range of the new text within the whole text after appending.

    @throw std::bad_alloc
        Thrown if memory allocation fails.
    */
    Range AppendText(std::wstring_view text, const TextStyle& style);

    void ClearText();

    const DefaultTextStyle& DefaultStyle() const {
        return default_style_;
    }

    void SetDefaultStyle(DefaultTextStyle default_style) {
        default_style_ = std::move(default_style);
    }

    const Font& DefaultFont() const {
        return default_style_.Font();
    }

    void SetDefaultFont(Font font) {
        default_style_.SetFont(std::move(font));
    }

    void SetFontInRange(Font font, const Range& range);
    void ClearRangedFonts();
    const Font& GetFontAtIndex(std::size_t index) const;
    const RangedFontAccessor& RangedFonts() const;
    RangedFontAccessor& RangedFonts();

    const Color& DefaultTextColor() const noexcept {
        return default_style_.TextColor();
    }

    void SetDefaultTextColor(const Color& color) {
        default_style_.SetTextColor(color);
    }

    void SetTextColorInRange(const Color& color, const Range& range);
    void ClearRangedTextColors();
    const Color& GetTextColorAtIndex(std::size_t index) const;
    const RangedColorAccessor& RangedTextColors() const;
    RangedColorAccessor& RangedTextColors();

    const Color& DefaultTextBackColor() const {
        return default_style_.TextBackColor();
    }

    void SetDefaultTextBackColor(const Color& color) {
        default_style_.SetTextBackColor(color);
    }

    void SetTextBackColorInRange(const Color& color, const Range& range);
    void ClearRangedTextBackColors();
    const Color& GetTextBackColorAtIndex(std::size_t index) const;
    const RangedColorAccessor& RangedTextBackColors() const;
    RangedColorAccessor& RangedTextBackColors();

    void AttachInlineObjectToRange(
        std::shared_ptr<InlineObject> object,
        const Range& range);

    void ClearInlineObjects();

    std::shared_ptr<InlineObject> GetInlineObjectAtIndex(std::size_t index) const;
    const InlineObjectAccessor& InlineObjects() const;

    /**
    Gets a sub-portion of the styled text.

    @param range
        The range of the sub-portion within the styled text. The end index may exceed the length of
        the styled text; in such a case, the sub-portion contains the text up to the end.

    @pre
        The start index of range does not exceed the length of the style text.

    @return
        The sub-portion of the styled text.

    @throw zaf::PreconditionError
        Thrown if the precondition is violated.

    @throw std::bad_alloc
        Thrown if fails to allocate the required memory.
    */
    StyledText GetSubText(const Range& range) const;

    Range SetStyledTextInRange(const StyledText& styled_text, const Range& range);

    StyledText Clone() const;

    void WriteToXML(XMLWriter& writer) const override;

private:
    static Range ReviseItemRangeForGettingSubText(
        const Range& item_range, 
        const Range& sub_text_range);

    static Range ReviseItemRangeForSettingSubText(
        const Range& item_range,
        const Range& sub_text_range);

    void CheckRange(const Range& range) const;

private:
    std::wstring text_;
    DefaultTextStyle default_style_;
    RangedTextStyle ranged_style_;
};

}