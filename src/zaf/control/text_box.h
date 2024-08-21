#pragma once

#include <zaf/base/range.h>
#include <zaf/control/self_scroll_control.h>
#include <zaf/graphic/text/text_layout.h>
#include <zaf/control/textual_control.h>
#include <zaf/control/textual/selection_option.h>
#include <zaf/control/textual/word_extractor.h>
#include <zaf/window/event/message_received_info.h>

namespace zaf {
namespace internal {
class TextBoxCaretManager;
class TextBoxHitTestManager;
class TextBoxModuleContext;
class TextBoxMouseInputHandler;
class TextBoxSelectionChangedInfo;
}

class TextBox : public TextualControl, public SelfScrollControl {
public:
    ZAF_OBJECT;

public:
    TextBox();
    ~TextBox();

    /**
    Indicates whether the text box is editable.

    @details
        A text box is not editable by default. A non-editable text box doesn't respond to user 
        input that would modify the text.
    */
    bool IsEditable() const noexcept;

    /**
    Sets whether the text box is editable.
    */
    void SetIsEditable(bool is_editable);

    /**
    Indicates whether the caret is enabled when the text box is not editable.

    @details
        The caret is disabled for a non-editable text box by default.
    */
    bool IsCaretEnabledWhenNotEditable() const noexcept;

    /**
    Sets whether the caret is enabled when the text box is not editable.
    */
    void SetIsCaretEnabledWhenNotEditable(bool value);

    /**
    Indicates whether the text box allows undo operations.

    @details
        A text box allows undo operations by default.
    */
    bool AllowUndo() const noexcept;

    /**
    Sets whether the text box allows undo operations.
    */
    void SetAllowUndo(bool allow_undo);

    /**
    Determines whether there are undoable text modification operations in the text box's history.
    */
    bool CanUndo() const noexcept;

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
    bool CanRedo() const noexcept;
    
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
        textual::SelectionOption selection_option = textual::SelectionOption::Default);

    Color SelectionBackColor() const noexcept;
    void SetSelectionBackColor(const Color& color);
    
    const ColorPicker& SelectionBackColorPicker() const noexcept;
    void SetSelectionBackColorPicker(ColorPicker picker);

    std::wstring SelectedText() const;

    /**
    Selects the completed word around the specified text index.

    @param index
        The index around which to select. It may exceed the length of the text.

    @param selection_option
        The option that controls the behavior when setting the selection range.

    @throw zaf::Error
        Thrown if the operation fails.

    @details
        This method uses WordExtractor() to determine the word range.
    */
    void SelectWordAtIndex(
        std::size_t index, 
        textual::SelectionOption selection_option = textual::SelectionOption::Default);

    /**
    Gets the word extractor used to determine the word range to be selected.

    @return 
        The word extractor. It is guaranteed to be not nullptr.
    */
    const textual::WordExtractor& WordExtractor() const noexcept;

    /**
    Sets the word extractor used to determine the word range to be selected.

    @param extractor
        The extractor to be set. If it is nullptr, the default extractor will be used.

    @details
        The word extractor is used when the user double clicks on the text box, or when
        SelectWordAtIndex() is called. You can set a custom word extractor function to replace the 
        default behavior of determining the completed word to select. If you want to disable the
        feature of selecting the word on double click, call SetCanDoubleClick() with false.
    */
    void SetWordExtractor(textual::WordExtractor extractor) noexcept;

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

    /**
    Copies the selected text to the clipboard.

    @return
        Returns true if the selected text is copied to the clipboard; or returns false if there is 
        no selected text.

    @throw zaf::COMError
        Thrown if fails to access the clipboard.

    @throw std::bad_alloc
        Thrown if memory allocation fails during the copying.

    @details
        This method is equivalent to press the Ctrl+C key combination.
    */
    bool Copy() const;

    /**
    Pastes the content of the clipboard into the text box.

    @return
        Returns true if the content of the clipboard is pasted into the text box. Returns false if
        the text box is not editable or the content is not accepted by the text box.

    @throw zaf::COMError
        Thrown if fails to access the clipboard.

    @throw std::bad_alloc
        Thrown if memory allocation fails during the pasting.

    @details
        This method is equivalent to press the Ctrl+V key combination.
    */
    bool Paste();

    /**
    Cuts the selected text to the clipboard.

    @return
        Returns true if the selected text is cut to the clipboard. Returns false if the text box is
        not editable or there is no selected text.

    @throw zaf::COMError
        Thrown if fails to access the clipboard.

    @throw std::bad_alloc
        Thrown if memory allocation fails during the cutting.

    @details
        This method is equivalent to press the Ctrl+X key combination.
    */
    bool Cut();

protected:
    void Initialize() override;
    void Layout(const zaf::Rect&) override;
    zaf::Rect DetermineTextRect() override;
    void OnTextChanged(const TextChangedInfo& event_info) override;
    void UpdateStyle() override;
    void Paint(Canvas& canvas, const zaf::Rect& dirty_rect) const override;
    void PaintTextBack(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout,
        const zaf::Rect& layout_rect) const override;
    void OnMouseCursorChanging(const MouseCursorChangingInfo& event_info) override;
    void OnMouseDown(const MouseDownInfo& event_info) override;
    void OnMouseMove(const MouseMoveInfo& event_info) override;
    void OnMouseUp(const MouseUpInfo& event_info) override;
    void OnDoubleClick(const DoubleClickInfo& event_info) override;
    void OnFocusGained(const FocusGainedInfo& event_info) override;
    void OnFocusLost(const FocusLostInfo& event_info) override;
    void OnKeyDown(const KeyDownInfo& event_info) override;
    void OnCharInput(const CharInputInfo& event_info) override;
    void OnIMEStartComposition(const IMEStartCompositionInfo& event_info) override;
    void OnIMEComposition(const IMECompositionInfo& event_info) override;

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
        int& page_value) const override;
    void GetHorizontalScrollValues(
        int& current_value,
        int& min_value,
        int& max_value,
        int& page_value) const override;

    Observable<SelfScrollControlScrollBarChangeInfo> ScrollBarChangeEvent() override;
    Observable<SelfScrollControlScrollValuesChangeInfo> ScrollValuesChangeEvent() override;
    void VerticallyScroll(int new_value) override;
    void HorizontallyScroll(int new_value) override;

private:
    friend class internal::TextBoxCaretManager;
    friend class internal::TextBoxHitTestManager;
    friend class internal::TextBoxModuleContext;

    void PaintSelection(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const TextLayout& text_layout,
        const zaf::Rect& layout_rect) const;

    void UpdateTextRectOnLayout();

    void OnSelectionChanged(const internal::TextBoxSelectionChangedInfo& event_info);
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
    std::unique_ptr<internal::TextBoxModuleContext> module_context_;

    zaf::Rect text_rect_;

    bool allow_vertical_scroll_{ true };
    bool allow_horizontal_scroll_{ true };
    bool auto_hide_scroll_bars_{ false };

    Event<SelfScrollControlScrollBarChangeInfo> scroll_bar_change_event_;
    Event<SelfScrollControlScrollValuesChangeInfo> scroll_values_change_event_;

    internal::ColorField selection_back_color_field_;
    textual::WordExtractor word_extractor_;

    Subscription ime_message_subscription_;
};

ZAF_OBJECT_BEGIN(TextBox);
ZAF_OBJECT_PROPERTY(AllowUndo);
ZAF_OBJECT_PROPERTY(CanRedo);
ZAF_OBJECT_PROPERTY(CanUndo);
ZAF_OBJECT_PROPERTY(IsCaretEnabledWhenNotEditable);
ZAF_OBJECT_PROPERTY(IsEditable);
ZAF_OBJECT_PROPERTY(SelectionBackColor);
ZAF_OBJECT_PROPERTY(SelectionBackColorPicker);
ZAF_OBJECT_PROPERTY(SelectedText);
ZAF_OBJECT_PROPERTY(SelectionRange);
ZAF_OBJECT_END;

}