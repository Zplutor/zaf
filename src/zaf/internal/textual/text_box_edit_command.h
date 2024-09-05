#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/textual/styled_text.h>
#include <zaf/internal/textual/text_box_module_context.h>

namespace zaf::internal {

class TextBoxEditCommand : NonCopyableNonMovable {
public:
    class SelectionInfo {
    public:
        /**
        Indicates whether the caret should be set to the beginning of the new text.
        */
        bool set_caret_to_begin{};

        /**
        Indicates whether the new text should be selected.
        */
        bool select_slice{};
    };

    class EditInfo {
    public:
        /**
        The range which will be replaced in the original text.
        */
        Range replaced_range;

        /**
        The new styled txt slice which will be set to replaced_range.
        */
        textual::StyledText styled_text_slice;
    };

public:
    TextBoxEditCommand(
        EditInfo edit_info, 
        const SelectionInfo& do_selection_info,
        const SelectionInfo& undo_selection_info);

    void Do(const TextBoxModuleContext& context);
    void Undo(const TextBoxModuleContext& context);

private:
    static EditInfo Execute(
        const TextBoxModuleContext& context, 
        EditInfo edit_info,
        const SelectionInfo& selection_info);

    static void SetEditInfoToText(
        const TextBoxModuleContext& context, 
        EditInfo edit_info,
        const SelectionInfo& selection_info,
        Range& new_text_range,
        textual::StyledText& old_text);

private:
    EditInfo edit_info_;
    SelectionInfo do_selection_info_;
    SelectionInfo undo_selection_info_;
    bool is_done_{};
};

}