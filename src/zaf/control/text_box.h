#pragma once

#include <zaf/control/textual_control.h>

namespace zaf {

class TextBox : public TextualControl {
public:
    ZAF_DECLARE_TYPE;

protected:
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;

private:
    void PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect);

private:
    bool is_selecting_{};
    std::size_t begin_select_index_{};
    std::size_t end_select_index_{};
};

}