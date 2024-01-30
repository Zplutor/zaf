#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/event/keyboard_event_info.h>
#include <zaf/control/text_box/internal/text_box_edit_command.h>
#include <zaf/control/text_box/internal/text_box_module.h>

namespace zaf::internal {

class TextBoxEditor : public TextBoxModule {
public:
    explicit TextBoxEditor(TextBoxModuleContext* context);

    void Initialize() override;

    void HandleKeyDown(const KeyDownInfo& event_info);
    void HandleCharInput(const CharInputInfo& event_info);

    bool IsEditing() const {
        return is_editing_;
    }

private:
    std::unique_ptr<TextBoxEditCommand> CreateCommandForDelete();
    std::unique_ptr<TextBoxEditCommand> CreateCommandForChar(wchar_t ch);
    std::unique_ptr<TextBoxEditCommand> CreateCommandForBackspace();

    void HandleCut();
    void HandlePaste();

    std::unique_ptr<TextBoxEditCommand> CreateCommand(
        std::wstring new_text,
        const Range& replaced_selection_range,
        const Range& new_selection_range) const;

    void ExecuteCommand(std::unique_ptr<TextBoxEditCommand> command);
    void HandleUndo();

private:
    bool is_editing_{};

    std::vector<std::unique_ptr<TextBoxEditCommand>> edit_commands_;
    std::size_t next_command_index_{};
};

}