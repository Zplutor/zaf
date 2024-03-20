#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/styled_text_slice.h>
#include <zaf/internal/textual/text_box_module_context.h>

namespace zaf::internal {

class TextBoxEditCommand : NonCopyableNonMovable {
public:
    class EditInfo {
    public:
        /**
        The range which will be replaced in the original text.
        */
        Range replaced_range;

        /**
        The new styled txt slice which will be set to replaced_range.
        */
        textual::StyledTextSlice styled_text_slice;

        /**
        The selection range which will be set after replacing.
        */
        Range new_selection_range;

        /**
        A value indicates whether the caret should be set to the beginning of new_selection_range.
        */
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