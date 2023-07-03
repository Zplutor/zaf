#pragma once

#include <zaf/base/range.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {
namespace internal {
class TextBoxCore;
}

class Caret;

class TextBox : public TextualControl {
public:
    ZAF_DECLARE_TYPE;

public:
    TextBox();

protected:
    void Initialize() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;

private:
    class TextIndexInfo {
    public:
        std::size_t index{};
        zaf::Rect rect{};
    };

private:
    friend class zaf::internal::TextBoxCore;

    //Called from TextBoxCore to paint selection background.
    void PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect);

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

    std::optional<TextIndexInfo> FindTextIndexAtPoint(const Point& point) const;

    void UpdateSelectionByMouse(const TextIndexInfo& index_info, bool begin_selection);

    TextLayout GetTextLayout() const;

private:
    std::optional<std::pair<std::size_t, std::size_t>> selecting_indexes_;
    Range selection_range_;

    std::shared_ptr<Caret> caret_;
};

}