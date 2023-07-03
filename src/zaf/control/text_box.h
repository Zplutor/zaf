#pragma once

#include <zaf/base/range.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/text/text_layout.h>

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
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;

private:
    friend class zaf::internal::TextBoxCore;

    //Called from TextBoxCore to paint selection background.
    void PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect);

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

    std::optional<std::size_t> FindTextIndexAtPoint(const Point& point) const;
    void DetermineSelectionRange();

    TextLayout GetTextLayout() const;

private:
    std::optional<std::pair<std::size_t, std::size_t>> selecting_indexes_;
    Range selection_range_;
};

}