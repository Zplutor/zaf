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

    void SimulatePaste();

    bool IsEditing() const {
        return is_editing_;
    }

private:
    std::unique_ptr<TextBoxEditCommand> HandleKey(Key key);
    std::unique_ptr<TextBoxEditCommand> HandleEnter();
    std::unique_ptr<TextBoxEditCommand> HandleDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchBackspace();
    std::unique_ptr<TextBoxEditCommand> HandleCut();
    std::unique_ptr<TextBoxEditCommand> HandlePaste();
    std::unique_ptr<TextBoxEditCommand> CreatePasteCommand() const;

    std::unique_ptr<TextBoxEditCommand> HandleChar(wchar_t ch);
    std::unique_ptr<TextBoxEditCommand> HandleBackspace();

    std::unique_ptr<TextBoxEditCommand> CreateInsertTextCommand(std::wstring new_text);

    std::unique_ptr<TextBoxEditCommand> CreateCommand(
        textual::StyledText new_text,
        const Range& replaced_selection_range,
        const Range& new_selection_range) const;

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