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
    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;

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

    std::wstring_view GetText() const;
    TextLayout GetTextLayout() const;

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

    std::optional<TextIndexInfo> FindTextIndexAtPoint(const Point& point) const;

    void UpdateSelectionByMouse(const TextIndexInfo& index_info, bool begin_selection);

    void BackwardCaretIndex(bool is_selecting_range);
    void ForwardCaretIndex(bool is_selecting_range);
    void UpdateCaretIndex(std::size_t caret_index, bool is_selecting_range);
    void UpdateCaretAtCurrentIndex();
    void ShowCaret(const zaf::Rect& caret_rect);

private:
    internal::TextBoxCore* core_{};

    std::optional<std::size_t> begin_mouse_select_index_;
    Range selection_range_;
    std::size_t caret_index_{};
    std::shared_ptr<Caret> caret_;
};

}