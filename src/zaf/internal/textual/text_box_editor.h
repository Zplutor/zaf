#pragma once

#include <optional>
#include <zaf/base/event/event.h>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/textual/pasting_info.h>
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

    /**
    @throw zaf::COMError
    @throw std::bad_alloc
    */
    bool PerformPaste();

    Observable<textual::PastingInfo> PastingEvent() const {
        return pasting_event_.GetObservable();
    }

    /**
    @throw zaf::COMError
    @throw std::bad_alloc
    */
    bool PerformCut();

    bool PerformInputText(std::wstring_view text);
    bool PerformInputStyledText(textual::StyledText styled_text);

    bool IsEditing() const {
        return is_editing_;
    }

private:
    void HandleEnter();

    std::unique_ptr<TextBoxEditCommand> HandleKey(Key key);
    std::unique_ptr<TextBoxEditCommand> HandleDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchDelete();
    std::unique_ptr<TextBoxEditCommand> HandleBatchBackspace();
    std::unique_ptr<TextBoxEditCommand> HandleBackspace();

    bool InnerPerformInputText(std::wstring text, bool can_truncate);
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
    bool can_edit_{};
    Event<bool> can_edit_changed_event_;

    bool allow_undo_{ true };
    bool is_editing_{};
    std::optional<std::size_t> max_length_;

    Event<textual::PastingInfo> pasting_event_;

    std::vector<std::unique_ptr<TextBoxEditCommand>> edit_commands_;
    std::size_t next_command_index_{};
};

}