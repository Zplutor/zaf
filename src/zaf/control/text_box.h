#pragma once

#include <zaf/base/range.h>
#include <zaf/control/color_picker.h>
#include <zaf/control/self_scroll_control.h>
#include <zaf/control/textual_control.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/control/text_box/selection_option.h>
#include <zaf/control/text_box/word_extractor.h>

namespace zaf {
namespace internal {
class TextBoxCore;
class TextBoxCaretIndexChangedInfo;
class TextBoxModuleContext;
}

class Caret;

class TextBox : public TextualControl, public SelfScrollControl {
public:
    ZAF_DECLARE_TYPE;

public:
    TextBox();
    ~TextBox();

    /**
    Indicates whether the text box is editable.

    @remark
        A text box is not editable by default. A non-editable text box doesn't respond to user 
        input that would modify the text.
    */
    bool IsEditable() const;

    /**
    Sets whether the text box is editable.
    */
    void SetIsEditable(bool is_editable);

    /**
    Determines whether there are undoable text modification operations in the text box's history.
    */
    bool CanUndo() const;

    /**
    Undoes the most recent text modification operation in the text box's history.

    @return
        Returns true if the operation is successfully undone; or returns false if there are no 
        operations in the text box's history.
    */
    bool Undo();

    /**
    Determines whether there are redoable text modification operations in the text box's history.
    */
    bool CanRedo() const;
    
    /**
    Redoes the most recently undone text modification operation in the text box's history.

    @return
        Returns true if the operation is successfully redone; or returns false if there are no 
        undone operations in the text box's history to redo.
    */
    bool Redo();

    std::size_t LineCount() const;

    std::size_t CaretIndex() const;

    const Range& SelectionRange() const;

    /**
    Sets the specified selected text range in the text box.

    @param range
        The range of the selection. The index and length may exceed the length of the text; in such
        case, the selection range will be adjusted to fit the text.

    @param selection_option
        The option that controls the behavior when setting the selection range.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    void SetSelectionRange(
        const Range& range,
        text_box::SelectionOption selection_option = text_box::SelectionOption::Default);

    ColorPicker SelectionBackgroundColorPicker() const;
    void SetSelectionBackgroundColorPicker(ColorPicker picker);

    Color SelectionBackgroundColor() const;
    void SetSelectionBackgroundColor(const Color& color);

    std::wstring SelectedText() const;

    /**
    Selects the completed word around the specified text index.

    @param index
        The index around which to select. It may exceed the length of the text.

    @param selection_option
        The option that controls the behavior when setting the selection range.

    @throw zaf::Error
        Thrown if the operation fails.

    @remark
        This method uses WordExtractor() to determine the word range.
    */
    void SelectWordAtIndex(
        std::size_t index, 
        text_box::SelectionOption selection_option = text_box::SelectionOption::Default);

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
        The position in the coordinate space of the text box.

    @return
        The nearest text index found.

    @throw zaf::Error
        Thrown if the operation fails.
    */
    std::size_t FindIndexAtPosition(const Point& position) const;

    /**
    Determines whether the specified position in the text box's coordinate space is inside the text
    string.

    @param position
        The position in the coordinate space of the text box.

    @return
        Returns true if the position is inside the text string; otherwise returns false.

    @throw af::Error
        Thrown if the operation fails.
    */
    bool IsPositionInsideText(const Point& position) const;

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
    void OnCharInput(const CharInputInfo& event_info) override;

    //Methods from SelfScrollControl
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

    Observable<SelfScrollControlScrollBarChangeInfo> ScrollBarChangeEvent() override;
    Observable<SelfScrollControlScrollValuesChangeInfo> ScrollValuesChangeEvent() override;
    void VerticallyScroll(int new_value) override;
    void HorizontallyScroll(int new_value) override;

    //Called from TextBoxCore to paint text background.
    virtual void PaintTextBackground(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout, 
        const zaf::Rect& layout_rect);

private:
    friend class zaf::internal::TextBoxCore;

    void PaintCaret(Canvas& canvas, const zaf::Rect& dirty_rect);

    std::wstring_view GetText() const;
    TextLayout GetTextLayout() const;

    void UpdateTextRectOnLayout();

    HitTestPointResult HitTestAtPosition(const Point& position) const;

    void UpdateCaretAtCurrentIndex();
    void OnCaretIndexChanged(const internal::TextBoxCaretIndexChangedInfo& event_info);
    void ShowCaret(const zaf::Rect& char_rect_at_caret);
    void EnsureCaretVisible(const zaf::Rect& char_rect_at_caret);
    
    static void GetScrollValues(
        float content_length,
        float text_length,
        float text_position,
        int& current_value,
        int& min_value,
        int& max_value,
        int& page_value);
    void DoScroll(int new_value, float content_length, float text_length, float& text_position);

private:
    internal::TextBoxCore* core_{};
    std::unique_ptr<internal::TextBoxModuleContext> module_context_;

    std::shared_ptr<Caret> caret_;

    zaf::Rect text_rect_;

    bool allow_vertical_scroll_{ true };
    bool allow_horizontal_scroll_{ true };
    bool auto_hide_scroll_bars_{ false };

    Event<SelfScrollControlScrollBarChangeInfo> scroll_bar_change_event_;
    Event<SelfScrollControlScrollValuesChangeInfo> scroll_values_change_event_;

    ColorPicker selection_background_color_picker_;
    text_box::WordExtractor word_extractor_;
};

}