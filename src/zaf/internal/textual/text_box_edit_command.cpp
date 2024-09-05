#include <zaf/internal/textual/text_box_edit_command.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/internal/textual/text_model.h>

namespace zaf::internal {

TextBoxEditCommand::TextBoxEditCommand(
    EditInfo edit_info,
    const SelectionInfo& do_selection_info,
    const SelectionInfo& undo_selection_info)
    : 
    edit_info_(std::move(edit_info)),
    do_selection_info_(do_selection_info),
    undo_selection_info_(undo_selection_info) {

}


void TextBoxEditCommand::Do(const TextBoxModuleContext& context) {
    ZAF_EXPECT(!is_done_);
    edit_info_ = Execute(context, std::move(edit_info_), do_selection_info_);
    is_done_ = true;
}


void TextBoxEditCommand::Undo(const TextBoxModuleContext& context) {
    ZAF_EXPECT(is_done_);
    edit_info_ = Execute(context, std::move(edit_info_), undo_selection_info_);
    is_done_ = false;
}


TextBoxEditCommand::EditInfo TextBoxEditCommand::Execute(
    const TextBoxModuleContext& context, 
    EditInfo edit_info,
    const SelectionInfo& selection_info) {

    Range new_text_range;
    textual::StyledText old_text;
    SetEditInfoToText(context, std::move(edit_info), selection_info, new_text_range, old_text);

    EditInfo undo_info;
    undo_info.replaced_range = new_text_range;
    undo_info.styled_text_slice = std::move(old_text);
    return undo_info;
}


void TextBoxEditCommand::SetEditInfoToText(
    const TextBoxModuleContext& context,
    EditInfo edit_info,
    const SelectionInfo& selection_info,
    Range& new_text_range,
    textual::StyledText& old_text) {

    new_text_range = context.TextModel().SetStyledTextInRange(
        std::move(edit_info.styled_text_slice),
        edit_info.replaced_range, 
        &old_text);

    Range selection_range;
    if (selection_info.select_slice) {
        selection_range = new_text_range;
    }
    else {
        if (selection_info.set_caret_to_begin) {
            selection_range.index = new_text_range.index;
        }
        else {
            selection_range.index = new_text_range.EndIndex();
        }
    }

    auto selection_option =
        selection_info.set_caret_to_begin ?
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

