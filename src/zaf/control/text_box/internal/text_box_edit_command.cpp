#include <zaf/control/text_box/internal/text_box_edit_command.h>
#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/control/text_box/internal/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxEditCommand::TextBoxEditCommand(EditInfo do_info, EditInfo undo_info) :
    do_info_(std::move(do_info)),
    undo_info_(std::move(undo_info)) {

}


void TextBoxEditCommand::Do(const TextBoxModuleContext& context) {
    Execute(context, do_info_);
}


void TextBoxEditCommand::Undo(const TextBoxModuleContext& context) {
    Execute(context, undo_info_);
}


void TextBoxEditCommand::Execute(const TextBoxModuleContext& context, const EditInfo& edit_info) {

    context.TextModel().SetTextInRange(
        edit_info.text, 
        edit_info.replaced_selection_range);

    auto selection_option =
        edit_info.is_caret_at_begin ?
        text_box::SelectionOption::SetCaretToBegin :
        text_box::SelectionOption::SetCaretToEnd;

    selection_option |= text_box::SelectionOption::ScrollToCaret;

    context.SelectionManager().SetSelectionRange(
        edit_info.new_selection_range,
        selection_option, 
        true);
}

}

