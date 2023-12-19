#pragma once

#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/self_scrolling_control.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/control/text_box/word_extractor.h>

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

    std::size_t CaretIndex() const;

    const Range& SelectionRange() const;
    void SetSelectionRange(const Range& range, bool scroll_to_selection = true);

    ColorPicker SelectionBackgroundColorPicker() const;
    void SetSelectionBackgroundColorPicker(ColorPicker picker);

    Color SelectionBackgroundColor() const;
    void SetSelectionBackgroundColor(const Color& color);

    std::wstring SelectedText() const;

    /**
    Selects the completed word around the specified text index.

    @param index
        The index around which to select. It may exceed the length of the text.

    @remark
        This method uses WordExtractor() to determine the word range.
    */
    void SelectWordAtIndex(std::size_t index);

    /**
    Gets the word extractor used to determine the word range to be selected.

    @return 
        The word extractor. It is guaranteed to be not nullptr.
    */
    const text_box::WordExtractor& WordExtractor() const noexcept;

    /**
    Sets the word extractor used to determine the word range to be selected.

    @param extractor
        The extractor to be set. If it is nullptr, the default extractor will be used.

    @remark
        The word extractor is used when the user double clicks on the text box, or when
        SelectWordAtIndex() is called. You can set a custom word extractor function to replace the 
        default behavior of determining the completed word to select. If you want to disable the
        feature of selecting the word on double click, call SetCanDoubleClick() with false.
    */
    void SetWordExtractor(text_box::WordExtractor extractor) noexcept;

    /**
    Finds the nearest text index at the specified position in the coordinate space of the text box.

    @param position
        The position in the coordinate spaces of the text box.

    @return
        The nearest text index found.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    std::size_t FindIndexAtPosition(const Point& position) const;

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
    void OnDoubleClick(const DoubleClickInfo& event_info) override;
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

    //Called from TextBoxCore to paint text background.
    virtual void PaintTextBackground(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout, 
        const zaf::Rect& layout_rect);

private:
    class LineInfo {
    public:
        std::size_t line_char_index{};
        float line_y{};
        float line_height{};
    };

private:
    friend class zaf::internal::TextBoxCore;

    void PaintCaret(Canvas& canvas, const zaf::Rect& dirty_rect);

    std::wstring_view GetText() const;
    TextLayout GetTextLayout() const;

    void UpdateTextRectOnLayout();

    void HandleMouseDown(const MouseDownInfo& event_info);
    void HandleMouseMove(const MouseMoveInfo& event_info);
    void HandleMouseUp(const MouseUpInfo& event_info);

    void HandleKeyDown(const KeyDownInfo& event_info);
    void BackwardCaretIndex(bool expand_selection);
    void ForwardCaretIndex(bool expand_selection);
    void UpwardCaretIndex(bool expand_selection);
    void DownwardCaretIndex(bool expand_selection);
    void UpdateCaretIndexVertically(bool is_downward, bool expand_selection);
    LineInfo LocateCurrentLineInfo();
    void SetCaretIndexByMouse(std::size_t new_index, bool begin_selection);
    void SetCaretIndexByKey(std::size_t caret_index, bool expand_selection, bool update_caret_x);
    void AfterSetCaretIndex(bool update_caret_x, bool ensure_caret_visible);
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
    void HandleSelectAll();

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
    text_box::WordExtractor word_extractor_;
};

}