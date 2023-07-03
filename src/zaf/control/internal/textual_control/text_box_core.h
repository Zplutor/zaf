#pragma once

#include <zaf/control/internal/textual_control/general_textual_core.h>
#include <zaf/control/text_box.h>

namespace zaf::internal {

class TextBoxCore : public GeneralTextualCore {
public:
    void Initialize(const std::shared_ptr<TextualControl>& owner) override;
    void PaintText(Canvas& canvas, const Rect& dirty_rect, const Rect& text_rect) override;

private:
    std::weak_ptr<TextBox> owner_;
};

}