#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/text_box.h>

namespace zaf::internal {

class TextBoxRenderFacet : NonCopyableNonMovable {
public:
    explicit TextBoxRenderFacet(TextBox& owner) noexcept;

    void PaintText(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const dwrite::TextLayout& text_layout,
        const zaf::Rect& layout_rect) const;
    
private:
    void PaintCompositionUnderline(
        Canvas& canvas, 
        const dwrite::TextLayout& text_layout,
        const zaf::Rect& layout_rect) const;

private:
    TextBox& owner_;
};

}