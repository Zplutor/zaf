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
        Range selection_range;
        bool caret_at_begin{};
    };

public:
    TextBoxEditCommand(EditInfo new_edit_info, EditInfo old_edit_info);

    void Do(const TextBoxModuleContext& context);
    void Undo(const TextBoxModuleContext& context);

private:
    static void Execute(
        const TextBoxModuleContext& context,
        const std::wstring& text,
        const Range& old_selection_range,
        const Range& new_selection_range,
        bool set_caret_to_begin);

private:
    EditInfo new_edit_info_;
    EditInfo old_edit_info_;
};

}