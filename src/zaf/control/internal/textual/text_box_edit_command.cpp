#include <zaf/control/internal/textual/text_box_edit_command.h>
#include <zaf/control/internal/textual/text_box_selection_manager.h>
#include <zaf/control/internal/textual/text_model.h>

namespace zaf::internal {

TextBoxEditCommand::TextBoxEditCommand(
    EditParams edit_params,
    const SelectionInfo& do_selection_info,
    const SelectionInfo& undo_selection_info) noexcept
    : 
    edit_params_(std::move(edit_params)),
    do_selection_info_(do_selection_info),
    undo_selection_info_(undo_selection_info) {

}


void TextBoxEditCommand::Do(
    TextBox& text_box, 
    std::optional<textual::StyledText> undo_styled_text) {

    ZAF_EXPECT(!is_done_);

    edit_params_ = Execute(
        text_box, 
        edit_params_, 
        do_selection_info_, 
        std::move(undo_styled_text));

    is_done_ = true;
}


void TextBoxEditCommand::Undo(TextBox& text_box) {

    ZAF_EXPECT(is_done_);
    edit_params_ = Execute(text_box, edit_params_, undo_selection_info_, std::nullopt);
    is_done_ = false;
}


TextBoxEditCommand::EditParams TextBoxEditCommand::Execute(
    TextBox& text_box,
    EditParams& edit_params,
    const SelectionInfo& selection_info,
    std::optional<textual::StyledText> undo_text) {

    Range new_text_range;
    auto old_text = std::move(undo_text);

    ApplyEditParamsToText(
        text_box, 
        edit_params, 
        selection_info, 
        new_text_range, 
        old_text ? nullptr : &old_text.emplace());

    EditParams undo_params;
    undo_params.replaced_range = new_text_range;
    undo_params.styled_text_slice = std::move(*old_text);
    return undo_params;
}


void TextBoxEditCommand::ApplyEditParamsToText(
    TextBox& text_box,
    EditParams& edit_params,
    const SelectionInfo& selection_info,
    Range& new_text_range,
    textual::StyledText* old_text) {

    new_text_range = text_box.InnerTextModel().SetStyledTextInRange(
        std::move(edit_params.styled_text_slice),
        edit_params.replaced_range, 
        old_text);

    Range selection_range;
    if (selection_info.select_slice) {
        selection_range = new_text_range;
    }
    else {
        if (selection_info.set_caret_to_begin) {
            selection_range.SetIndex(new_text_range.Index());
        }
        else {
            selection_range.SetIndex(new_text_range.EndIndex());
        }
    }

    auto selection_option =
        selection_info.set_caret_to_begin ?
        textual::SelectionOption::SetCaretToBegin :
        textual::SelectionOption::SetCaretToEnd;

    selection_option |= textual::SelectionOption::ScrollToCaret;

    text_box.SelectionManager().SetSelectionRange(
        selection_range,
        selection_option,
        std::nullopt,
        true);
}

}

