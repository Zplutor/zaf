#pragma once

#include <optional>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/internal/textual/text_box_edit_command.h>
#include <zaf/internal/textual/text_box_module.h>
#include <zaf/rx/subscription_host.h>

namespace zaf::internal {

class TextBoxEditor : public TextBoxModule, SubscriptionHost {
public:
    explicit TextBoxEditor(TextBoxModuleContext* context);

    void Initialize() override;

    bool CanEdit() const noexcept {
        return can_edit_;
    }

    void SetCanEdit(bool can_edit);

    Observable<bool> CanEditChangedEvent() const {
        return can_edit_changed_event_.GetObservable();
    }

    bool AllowUndo() const noexcept {
        return allow_undo_;
    }

    void SetAllowUndo(bool allow_undo);

    bool CanUndo() const noexcept;
    bool Undo();

    bool CanRedo() const noexcept;
    bool Redo();

    void HandleKeyDown(const KeyDownInfo& event_info);
    void HandleCharInput(const CharInputInfo& event_info);

    /**
    Performs a paste operation to paste the content in the clipboard to the text box.

    @return
        Returns true if the content in the clipboard is pasted to the text box. Returns false if 
        the text box is not editable or the content is not accepted by the text box.

    @throw zaf::COMError
    @throw std::bad_alloc
    */
    bool PerformPaste();

    /**
    Performs a cut operation to cut the selected text to the clipboard.

    @return
        Returns true if the selected text is cut to the clipboard. Returns false if the text box
        is not editable or there is no selected text.

    @throw zaf::COMError
    @throw std::bad_alloc
    */
    bool PerformCut();

    bool IsEditing() const {
        return is_editing_;
    }

private:
    std::unique_ptr<TextBoxEditCommand> HandleKey(Key key);
    std::unique_ptr<TextBoxEditCommand> HandleEnter();
    std::unique_ptr<TextBoxEditCommand> HandleDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchBackspace();
    void HandleCut();
    bool InnerCut();
    void HandlePaste();
    bool InnerPaste();
    std::unique_ptr<TextBoxEditCommand> HandleChar(wchar_t ch);
    std::unique_ptr<TextBoxEditCommand> HandleBackspace();

    std::unique_ptr<TextBoxEditCommand> CreateInsertTextCommand(std::wstring new_text);

    std::unique_ptr<TextBoxEditCommand> CreateCommand(
        textual::StyledText new_text,
        const Range& replaced_selection_range,
        bool set_caret_to_begin) const;

    void ExecuteCommand(std::unique_ptr<TextBoxEditCommand> command);
    bool HandleUndo();
    bool HandleRedo();

    void OnTextModelChanged();
    void ClearCommands();

private:
    bool can_edit_{};
    Event<bool> can_edit_changed_event_;

    bool allow_undo_{ true };
    bool is_editing_{};

    std::vector<std::unique_ptr<TextBoxEditCommand>> edit_commands_;
    std::size_t next_command_index_{};
};

}