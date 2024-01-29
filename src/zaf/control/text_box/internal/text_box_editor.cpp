#include <zaf/control/text_box/internal/text_box_editor.h>
#include <cwctype>
#include <zaf/base/auto_reset.h>
#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/control/text_box/internal/text_box_core.h>
#include <zaf/control/text_box/internal/text_box_module_context.h>
#include <zaf/control/text_box/internal/text_box_selection_manager.h>

namespace zaf::internal {
namespace {

bool ShouldIgnoreChar(wchar_t ch) {

    //Some control chars in ASCII.
    switch (ch) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27: //ESC
    case 28:
    case 29:
    case 30:
    case 31:
        return true;
    default:
        return false;
    }
}

}

TextBoxEditor::TextBoxEditor(TextBoxModuleContext* context) : TextBoxModule(context) {

}


void TextBoxEditor::HandleKeyDown(const KeyDownInfo& event_info) {

    if (event_info.Message().VirtualKey() == VK_DELETE) {
        HandleDeleteKeyDown();
    }
}


void TextBoxEditor::HandleDeleteKeyDown() {

    auto auto_reset = MakeAutoReset(is_editing_, true);

    const auto& selection_range = Context().SelectionManager().SelectionRange();
    auto new_selection_range = HandleDelete(selection_range);
    if (new_selection_range) {

        Context().SelectionManager().SetSelectionRange(
            *new_selection_range,
            text_box::SelectionOption::ScrollToCaret,
            true);
    }
}


std::optional<Range> TextBoxEditor::HandleDelete(const Range& selection_range) {

    //Remove the selected text.
    if (selection_range.length > 0) {
        Context().Core().GetTextModel()->SetTextInRange({}, selection_range);
        return Range{ selection_range.index, 0 };
    }

    if (selection_range.EndIndex() == Context().Core().GetTextLength()) {
        return std::nullopt;
    }

    //Remove the current char.
    Context().Core().GetTextModel()->SetTextInRange({}, Range{ selection_range.index, 1 });
    return std::nullopt;
}


void TextBoxEditor::HandleCharInput(const CharInputInfo& event_info) {

    auto auto_reset = MakeAutoReset(is_editing_, true);

    auto ch = event_info.Message().Char();
    auto new_selection_range = HandleChar(ch);
    if (new_selection_range) {

        Context().SelectionManager().SetSelectionRange(
            *new_selection_range,
            text_box::SelectionOption::ScrollToCaret,
            true);
    }
}


std::optional<Range> TextBoxEditor::HandleChar(wchar_t ch) {

    //Ignore specific control chars.
    if (ShouldIgnoreChar(ch)) {
        return std::nullopt;
    }

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Backspace
    if (ch == '\x8') {
        return HandleBackspace(selection_range);
    }

    std::wstring new_text(1, ch);
    Context().Core().GetTextModel()->SetTextInRange(new_text, selection_range);
    return Range{ selection_range.index + 1, 0 };
}


std::optional<Range> TextBoxEditor::HandleBackspace(const Range& selection_range) {

    //Remove the selected text.
    if (selection_range.length > 0) {
        Context().Core().GetTextModel()->SetTextInRange({}, selection_range);
        return Range{ selection_range.index, 0 };
    }

    if (selection_range.index == 0) {
        return std::nullopt;
    }

    //Remove the previous char.
    Context().Core().GetTextModel()->SetTextInRange({}, Range{ selection_range.index - 1, 1 });
    return Range{ selection_range.index - 1, 0 };
}

}