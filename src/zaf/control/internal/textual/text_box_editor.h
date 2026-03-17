#pragma once

#include <optional>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/text_box.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/textual/pasting_info.h>
#include <zaf/control/internal/textual/text_box_edit_command.h>
#include <zaf/rx/disposable_host.h>

namespace zaf::internal {

/**
Responsible for handling text editing operations of a text box, such as inputting text, pasting 
content, and performing undo/redo operations.
*/
class TextBoxEditor : rx::DisposableHost {
public:
    explicit TextBoxEditor(TextBox& owner);

    void Initialize();

    bool CanEdit() const noexcept {
        return can_edit_;
    }

    void SetCanEdit(bool can_edit);

    rx::Observable<bool> CanEditChangedEvent() const {
        return can_edit_changed_event_.GetObservable();
    }

    std::optional<std::size_t> MaxLength() const noexcept {
        return max_length_;
    }

    void SetMaxLength(std::size_t max_length) noexcept {
        max_length_ = max_length;
    }

    bool AllowUndo() const noexcept {
        return allow_undo_;
    }

    void SetAllowUndo(bool allow_undo);

    bool CanUndo() const noexcept;
    bool PerformUndo();

    bool CanRedo() const noexcept;
    bool PerformRedo();

    void HandleKeyDown(const KeyDownInfo& event_info);
    void HandleCharInput(const CharInputInfo& event_info);
    void HandleIMEStartComposition(const IMEStartCompositionInfo& event_info);
    void HandleIMEComposition(const IMECompositionInfo& event_info);
    void HandleIMEEndComposition(const IMEEndCompositionInfo& event_info);
    void HandleFocusLost();

    /**
    @throw zaf::COMError
    @throw std::bad_alloc
    */
    bool PerformPaste();

    rx::Observable<textual::PastingInfo> PastingEvent() const {
        return pasting_event_.GetObservable();
    }

    /**
    @throw zaf::COMError
    @throw std::bad_alloc
    */
    bool PerformCut();

    bool PerformInputText(std::wstring_view text);
    bool PerformInputInlineObject(std::shared_ptr<textual::InlineObject> inline_object);
    bool PerformInputStyledText(textual::StyledText styled_text);

    bool IsPerformingEdit() const noexcept {
        return is_performing_edit_;
    }

private:
    void HandleEnter();

    std::unique_ptr<TextBoxEditCommand> HandleKey(Key key);
    std::unique_ptr<TextBoxEditCommand> HandleDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchBackspace();
    std::unique_ptr<TextBoxEditCommand> HandleBackspace();

    void UpdateCompositionText(std::wstring text);
    void ClearCompositionState();

    bool InnerPerformInputText(std::wstring_view text, bool can_truncate);
    void FillTextStyleFromSelection(textual::StyledText& styled_text) const;
    bool InnerPerformInputStyledText(textual::StyledText styled_text, bool can_truncate);
    bool InputStyledText(textual::StyledText styled_text, bool can_truncate);
    bool EnforceMaxLength(
        textual::StyledText& styled_text, 
        const Range& selection_range,
        bool can_truncate) const;

    std::unique_ptr<TextBoxEditCommand> CreateCommand(
        textual::StyledText new_text,
        const Range& replaced_selection_range,
        bool set_caret_to_begin) const;

    void ExecuteCommand(std::unique_ptr<TextBoxEditCommand> command);

    void OnTextModelChanged();
    void ClearCommands();

private:
    class CompositionState {
    public:
        Range composition_range;
    };

private:
    TextBox& owner_;
    bool can_edit_{ true };
    Event<bool> can_edit_changed_event_;

    std::optional<CompositionState> composition_state_;

    bool allow_undo_{ true };
    bool is_performing_edit_{};
    std::optional<std::size_t> max_length_;

    Event<textual::PastingInfo> pasting_event_;

    std::vector<std::unique_ptr<TextBoxEditCommand>> edit_commands_;
    std::size_t next_command_index_{};
};

}
