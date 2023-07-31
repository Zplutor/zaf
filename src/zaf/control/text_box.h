#pragma once

#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/text/text_layout.h>

namespace zaf {
namespace internal {
class TextBoxCore;
}

class Caret;

/**
A readonly control to display and select text.
*/
class TextBox : public TextualControl, public SelfScrollingControl {
public:
    ZAF_DECLARE_TYPE;

public:
    TextBox();

    std::size_t LineCount() const;

    const Range& SelectionRange() const;
    void SetSelectionRange(const Range& range);

    ColorPicker SelectionBackgroundColorPicker() const;
    void SetSelectionBackgroundColorPicker(ColorPicker picker);

    Color SelectionBackgroundColor() const;
    void SetSelectionBackgroundColor(const Color& color);

    std::wstring SelectedText() const;

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    zaf::Rect DetermineTextRect() override;
    void OnTextChanged(const TextChangedInfo& event_info) override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) override;
    void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;

    //Methods from SelfScrollingControl
    void SetAllowVerticalScroll(bool allow) override;
    void SetAllowHorizontalScroll(bool allow) override;
    void SetAutoHideScrollBars(bool auto_hide) override;
    bool CanShowVerticalScrollBar() override;
    bool CanShowHorizontalScrollBar() override;
    bool CanEnableVerticalScrollBar() override;
    bool CanEnableHorizontalScrollBar() override;
    void GetVerticalScrollValues(
        int& current_value,
        int& min_value,
        int& max_value,
        int& page_value) override;
    void GetHorizontalScrollValues(
        int& current_value,
        int& min_value,
        int& max_value,
        int& page_value) override;

    Observable<SelfScrollingControlScrollBarChangInfo> ScrollBarChangeEvent() override;
    Observable<SelfScrollingControlScrollValuesChangeInfo> ScrollValuesChangeEvent() override;
    void VerticallyScroll(int new_value) override;
    void HorizontallyScroll(int new_value) override;

private:
    class LineInfo {
    public:
        std::size_t line_char_index{};
        float line_y{};
        float line_height{};
    };

private:
    friend class zaf::internal::TextBoxCore;

    //Called from TextBoxCore to paint selection background.
    void PaintSelectionBackground(Canvas& canvas, const zaf::Rect& dirty_rect);

    void PaintCaret(Canvas& canvas, const zaf::Rect& dirty_rect);

    std::wstring_view GetText() const;
    TextLayout GetTextLayout() const;

    void UpdateTextRectOnLayout();

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);
    std::optional<std::size_t> FindTextIndexAtPoint(const Point& point_in_control) const;

    void HandleKeyDown(const KeyDownInfo& event_info);
    void BackwardCaretIndex(bool expand_selection);
    void ForwardCaretIndex(bool expand_selection);
    void UpwardCaretIndex(bool expand_selection);
    void DownwardCaretIndex(bool expand_selection);
    void UpdateCaretIndexVertically(bool is_downward, bool expand_selection);
    LineInfo LocateCurrentLineInfo();
    void SetCaretIndexByMouse(std::size_t new_index, bool begin_selection);
    void SetCaretIndexByKey(std::size_t caret_index, bool expand_selection, bool update_caret_x);
    void AfterSetCaretIndex(bool update_caret_x);
    void UpdateCaretAtCurrentIndex();
    void ShowCaret(const HitTestMetrics& metrics);
    void EnsureCaretVisible(const HitTestMetrics& metrics);
    
    static void GetScrollValues(
        float content_length,
        float text_length,
        float text_position,
        int& current_value,
        int& min_value,
        int& max_value,
        int& page_value);
    void DoScroll(int new_value, float content_length, float text_length, float& text_position);

    void HandleCopy();

private:
    internal::TextBoxCore* core_{};

    std::optional<std::size_t> begin_mouse_select_index_;
    Range selection_range_;
    std::size_t caret_index_{};
    std::shared_ptr<Caret> caret_;
    float caret_last_x_{};

    zaf::Rect text_rect_;

    bool allow_vertical_scroll_{ true };
    bool allow_horizontal_scroll_{ true };
    bool auto_hide_scroll_bars_{ false };

    Event<SelfScrollingControlScrollBarChangInfo> scroll_bar_change_event_;
    Event<SelfScrollingControlScrollValuesChangeInfo> scroll_values_change_event_;

    ColorPicker selection_background_color_picker_;
};

}