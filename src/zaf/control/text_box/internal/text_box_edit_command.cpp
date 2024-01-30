#include <zaf/control/text_box/internal/text_box_edit_command.h>
#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/control/text_box/internal/text_box_core.h>
#include <zaf/control/text_box/internal/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxEditCommand::TextBoxEditCommand(EditInfo new_edit_info, EditInfo old_edit_info) :
    new_edit_info_(std::move(new_edit_info)),
    old_edit_info_(std::move(old_edit_info)) {

}


void TextBoxEditCommand::Do(const TextBoxModuleContext& context) {

    Execute(
        context,
        new_edit_info_.text,
        old_edit_info_.selection_range,
        new_edit_info_.selection_range,
        new_edit_info_.caret_at_begin);
}


void TextBoxEditCommand::Undo(const TextBoxModuleContext& context) {

    Execute(
        context,
        old_edit_info_.text, 
        new_edit_info_.selection_range,
        old_edit_info_.selection_range,
        old_edit_info_.caret_at_begin);
}


void TextBoxEditCommand::Execute(
    const TextBoxModuleContext& context,
    const std::wstring& text, 
    const Range& old_selection_range, 
    const Range& new_selection_range,
    bool set_caret_to_begin) {

    context.Core().GetTextModel()->SetTextInRange(text, old_selection_range);

    auto selection_option =
        set_caret_to_begin ?
        text_box::SelectionOption::SetCaretToBegin :
        text_box::SelectionOption::SetCaretToEnd;

    selection_option |= text_box::SelectionOption::ScrollToCaret;

    context.SelectionManager().SetSelectionRange(new_selection_range, selection_option, true);
}

}

