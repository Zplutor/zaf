#include <zaf/internal/textual/text_box_edit_command.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/internal/textual/text_model.h>

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

    auto new_range = context.TextModel().ReplaceStyledTextSlice(
        edit_info.replaced_range,
        edit_info.styled_text_slice);

    Range selection_range;
    if (edit_info.select_slice) {
        selection_range = new_range;
    }
    else {
        if (edit_info.set_caret_to_begin) {
            selection_range.index = new_range.index;
        }
        else {
            selection_range.index = new_range.EndIndex();
        }
    }

    auto selection_option =
        edit_info.set_caret_to_begin ?
        textual::SelectionOption::SetCaretToBegin :
        textual::SelectionOption::SetCaretToEnd;

    selection_option |= textual::SelectionOption::ScrollToCaret;

    context.SelectionManager().SetSelectionRange(
        selection_range,
        selection_option, 
        std::nullopt,
        true);
}

}

