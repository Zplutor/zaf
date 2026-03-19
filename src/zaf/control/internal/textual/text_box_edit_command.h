#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/base/range.h>
#include <zaf/control/text_box.h>
#include <zaf/control/textual/styled_text.h>

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

    class EditParams {
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
        EditParams edit_params,
        const SelectionInfo& do_selection_info,
        const SelectionInfo& undo_selection_info) noexcept;

    void Do(TextBox& text_box, std::optional<textual::StyledText> undo_styled_text = std::nullopt);
    void Undo(TextBox& text_box);

    const EditParams& GetEditParams() const noexcept {
        return edit_params_;
    }

private:
    static EditParams Execute(
        TextBox& text_box,
        EditParams& edit_params,
        const SelectionInfo& selection_info,
        std::optional<textual::StyledText> undo_text);

    static void ApplyEditParamsToText(
        TextBox& text_box,
        EditParams& edit_params,
        const SelectionInfo& selection_info,
        Range& new_text_range,
        textual::StyledText* old_text);

private:
    EditParams edit_params_;
    const SelectionInfo do_selection_info_;
    const SelectionInfo undo_selection_info_;
    bool is_done_{};
};

}
