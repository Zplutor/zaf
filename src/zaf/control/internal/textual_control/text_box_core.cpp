#include <zaf/control/internal/textual_control/text_box_core.h>
#include <zaf/base/as.h>

namespace zaf::internal {

void TextBoxCore::Initialize(const std::shared_ptr<TextualControl>& owner) {

    __super::Initialize(owner);

    owner_ = As<TextBox>(owner);
}


void TextBoxCore::PaintTextLayout(
    Canvas& canvas, 
    const Rect& dirty_rect, 
    const TextLayout& text_layout, 
    const Rect& layout_rect) {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    //Paint background before painting text.
    owner->PaintTextBackground(canvas, dirty_rect, text_layout, layout_rect);

    __super::PaintTextLayout(canvas, dirty_rect, text_layout, layout_rect);
}

}