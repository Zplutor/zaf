#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {
namespace internal {
class TextBoxCore;
}

class TextBox : public TextualControl {
public:
    ZAF_DECLARE_TYPE;

public:
    TextBox();

protected:
    void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;

private:
    friend class zaf::internal::TextBoxCore;

    //Called from TextBoxCore to paint selection background.
    void PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect);

private:
    bool is_selecting_{};
    std::size_t begin_select_index_{};
    std::size_t end_select_index_{};
};

}