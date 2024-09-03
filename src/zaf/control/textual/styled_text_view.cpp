#include <zaf/control/textual/styled_text_view.h>
#include <zaf/control/textual/styled_text.h>

namespace zaf::textual {

StyledTextView::StyledTextView(const StyledText& styled_text, const Range& view_range) :
    styled_text_(styled_text),
    view_range_(view_range) {

    ZAF_EXPECT(view_range.index <= styled_text.Length());

    if (view_range_.EndIndex() > styled_text.Length()) {
        view_range_.length = styled_text.Length() - view_range_.index;
    }
}


std::wstring_view StyledTextView::Text() const noexcept {
    return std::wstring_view{ 
        styled_text_.Text().data() + view_range_.index,
        view_range_.length 
    };
}


const DefaultTextStyle& StyledTextView::DefaultStyle() const noexcept {
    return styled_text_.DefaultStyle();
}


const Font& StyledTextView::DefaultFont() const noexcept {
    return styled_text_.DefaultFont();
}


const Font& StyledTextView::GetFontAtIndex(std::size_t index) const {
    ZAF_EXPECT(index <= view_range_.length);
    return styled_text_.GetFontAtIndex(index + view_range_.index);
}


StyledTextView::RangedFontView StyledTextView::RangedFonts() const noexcept {
    return RangedFontView{ styled_text_.RangedFonts(), view_range_ };
}


const Color& StyledTextView::DefaultTextColor() const noexcept {
    return styled_text_.DefaultTextColor();
}


const Color& StyledTextView::GetTextColorAtIndex(std::size_t index) const {
    ZAF_EXPECT(index <= view_range_.length);
    return styled_text_.GetTextColorAtIndex(index + view_range_.index);
}


StyledTextView::RangedColorView StyledTextView::RangedTextColors() const noexcept {
    return RangedColorView{ styled_text_.RangedTextColors(), view_range_ };
}


const Color& StyledTextView::DefaultTextBackColor() const noexcept {
    return styled_text_.DefaultTextBackColor();
}


const Color& StyledTextView::GetTextBackColorAtIndex(std::size_t index) const {
    ZAF_EXPECT(index <= view_range_.length);
    return styled_text_.GetTextBackColorAtIndex(index + view_range_.index);
}


StyledTextView::RangedColorView StyledTextView::RangedTextBackColors() const noexcept {
    return RangedColorView{ styled_text_.RangedTextBackColors(), view_range_ };
}


std::shared_ptr<InlineObject> StyledTextView::GetInlineObjectAtIndex(std::size_t index) const {
    ZAF_EXPECT(index <= view_range_.length);
    return styled_text_.GetInlineObjectAtIndex(index + view_range_.index);
}


InlineObjectView StyledTextView::InlineObjects() const noexcept {
    return InlineObjectView{ styled_text_.InlineObjects(), view_range_ };
}

}