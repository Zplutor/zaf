#include <zaf/control/internal/textual_control/text_box_core.h>
#include <zaf/base/as.h>

namespace zaf::internal {

void TextBoxCore::Initialize(const std::shared_ptr<TextualControl>& owner) {

    __super::Initialize(owner);

    owner_ = As<TextBox>(owner);
}


void TextBoxCore::PaintText(Canvas& canvas, const Rect& dirty_rect) {

    auto owner = owner_.lock();
    if (!owner) {
        return;
    }

    //Paint the selection background before painting text.
    owner->PaintSelectionBackground(canvas, dirty_rect);

    __super::PaintText(canvas, dirty_rect);
}

}