#pragma once

/**
@file
    Defines the zaf::TextBox class.
*/

#include <zaf/base/range.h>
#include <zaf/control/self_scroll_control.h>
#include <zaf/graphic/dwrite/text_layout.h>
#include <zaf/control/textual_control.h>
#include <zaf/control/textual/copying_info.h>
#include <zaf/control/textual/pasting_info.h>
#include <zaf/control/textual/hit_test_index_result.h>
#include <zaf/control/textual/selection_changed_info.h>
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
        A text box is editable by default. A non-editable text box doesn't respond to user input
        that would modify the text.
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
    Gets the maximum number of characters that can be manually inputted into the text box.

    @return
        The maximum number of characters. Maybe null if there is no limit.
    */
    std::optional<std::size_t> MaxLength() const noexcept;

    /**
    Sets the maximum number of characters that can be manually inputted into the text box.

    @param max_length
        The maximum number of characters.
    */
    void SetMaxLength(std::size_t max_length) noexcept;

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

    Observable<textual::SelectionChangedInfo> SelectionChangedEvent() const;

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
    Performs a hit test at the specified character index of the existing text.

    @param index
        The index to hit test.

    @pre
        The index must be less than or equal to the length of the existing text.

    @return
        The hit test result that contains information of the character, such as its rect in the 
        text box.

    @throw zaf::PreconditionError

    @throw zaf::COMError
        Thrown if fails to perform the hit test.
    */
    textual::HitTestIndexResult HitTestAtIndex(std::size_t index) const;

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
    Gets the copying event.

    @details
        This event is raised before the selected text is copied to the clipboard. Users can 
        subscribe to the event to customize the behavior of the copying operation, and call 
        MarkAsHandled() of the event info to prevent the default copying operation.
    */
    Observable<textual::CopyingInfo> CopyingEvent() const;

    /**
    Pastes the content of the clipboard into the text box.

    @return
        Returns true if the content of the clipboard is pasted into the text box. Returns false if:
        - the text box is not editable;
        - the content is not accepted by the text box;
        - no content is pasted as the max length of the text box is reached.

    @throw zaf::COMError
        Thrown if fails to access the clipboard.

    @throw std::bad_alloc
        Thrown if memory allocation fails during the pasting.

    @details
        This method is equivalent to press the Ctrl+V key combination. The pasting operation will 
        be added to the undo history.
    */
    bool Paste();

    Observable<textual::PastingInfo> PastingEvent() const;

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
        This method is equivalent to press the Ctrl+X key combination. The cutting operation will 
        be added to the undo history.
    */
    bool Cut();

    /**
    Inputs the specified text to the text box.

    @param text
        The text to input.

    @return
        Returns true if the specified text is inputted to the text box. Returns false if:
        - the text box is not editable;
        - the text is empty;
        - no text is inputted as the max length of the text box is reached.

    @throw std::bad_alloc

    @details
        This method is equivalent to press keys to input the text. The text will be inserted at the 
        caret, or replaced the selected text. The input operation will be added to the undo
        history.
    */
    bool Input(std::wstring_view text);

    /**
    Inputs the specified styled text to the text box.

    @param styled_text
        The styled text to input.

    @return
        Returns true if the specified styled text is inputted to the text box. Returns false if:
        - the text box is not editable;
        - the styled text is empty;
        - no text is inputted as the max length of the text box is reached.

    @throw std::bad_alloc

    @details
        This method is equivalent to press keys to input the styled text. The styled text will be
        inserted at the caret, or replaced the selected text. The input operation will be added to 
        the undo history.
    */
    bool Input(textual::StyledText styled_text);

    bool Input(std::shared_ptr<textual::InlineObject> inline_object);

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
        const dwrite::TextLayout& text_layout,
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

    virtual void OnSelectionChanged(const textual::SelectionChangedInfo& event_info);

    /**
    Handles the copying event.

    @details
        The default implementation raises the copying event.
    */
    virtual void OnCopying(const textual::CopyingInfo& event_info);

    virtual void OnPasting(const textual::PastingInfo& event_info);

private:
    friend class internal::TextBoxCaretManager;
    friend class internal::TextBoxHitTestManager;
    friend class internal::TextBoxModuleContext;

    void PaintSelection(
        Canvas& canvas,
        const zaf::Rect& dirty_rect,
        const dwrite::TextLayout& text_layout,
        const zaf::Rect& layout_rect) const;

    void UpdateTextRectOnLayout();

    void OnInnerSelectionChanged(const internal::TextBoxSelectionChangedInfo& event_info);
    void OnInnerCopying(const textual::CopyingInfo& event_info);
    void OnInnerPasting(const textual::PastingInfo& event_info);
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

    Event<textual::SelectionChangedInfo> selection_changed_event_;
    Event<textual::CopyingInfo> copying_event_;
    Event<textual::PastingInfo> pasting_event_;
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