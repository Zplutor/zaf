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

    ZAF_EXPECT(edit_info.replaced_range.index == edit_info.styled_text_slice.Index());

    context.TextModel().ReplaceStyledTextSlice(
        edit_info.replaced_range,
        edit_info.styled_text_slice);

    auto selection_option =
        edit_info.is_caret_at_begin ?
        textual::SelectionOption::SetCaretToBegin :
        textual::SelectionOption::SetCaretToEnd;

    selection_option |= textual::SelectionOption::ScrollToCaret;

    context.SelectionManager().SetSelectionRange(
        edit_info.new_selection_range,
        selection_option, 
        std::nullopt,
        true);
}

}

