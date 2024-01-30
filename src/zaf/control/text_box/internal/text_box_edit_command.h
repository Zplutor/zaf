#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/text_box/internal/text_box_module_context.h>

namespace zaf::internal {

class TextBoxEditCommand : NonCopyableNonMovable {
public:
    class EditInfo {
    public:
        std::wstring text;
        Range replaced_selection_range;
        Range new_selection_range;
        bool is_caret_at_begin{};
    };

public:
    TextBoxEditCommand(EditInfo do_info, EditInfo undo_info);

    void Do(const TextBoxModuleContext& context);
    void Undo(const TextBoxModuleContext& context);

private:
    static void Execute(const TextBoxModuleContext& context, const EditInfo& edit_info);

private:
    EditInfo do_info_;
    EditInfo undo_info_;
};

}