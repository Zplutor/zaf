#include <zaf/control/text_box/internal/text_box_editor.h>
#include <cwctype>
#include <zaf/base/auto_reset.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/internal/textual_control/text_model.h>
#include <zaf/control/text_box/internal/text_box_core.h>
#include <zaf/control/text_box/internal/text_box_module_context.h>
#include <zaf/control/text_box/internal/text_box_selection_manager.h>
#include <zaf/input/keyboard.h>

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


void TextBoxEditor::Initialize() {

}


void TextBoxEditor::HandleKeyDown(const KeyDownInfo& event_info) {

    auto auto_reset = MakeAutoReset(is_editing_, true);

    auto key = event_info.Message().Key();
    if (key == Key::Delete) {
        HandleDeleteKeyDown();
    }
    else if ((key == Key::X) && Keyboard::IsCtrlDown()) {
        HandleCut();
    }
    else if ((key == Key::V) && Keyboard::IsCtrlDown()) {
        HandlePaste();
    }
    else if ((key == Key::Z) && Keyboard::IsCtrlDown()) {
        HandleUndo();
    }
}


void TextBoxEditor::HandleDeleteKeyDown() {

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
    auto command = CreateCommandForChar(ch);
    if (!command) {
        return;
    }

    ExecuteCommand(std::move(command));
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateCommandForChar(wchar_t ch) {

    //Ignore specific control chars.
    if (ShouldIgnoreChar(ch)) {
        return nullptr;
    }

    //Backspace
    if (ch == '\x8') {
        return CreateCommandForBackspace();
    }

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    return CreateCommand(
        std::wstring(1, ch),
        selection_range,
        Range{ selection_range.index + 1, 0 });
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateCommandForBackspace() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, Range{ selection_range.index, 0 });
    }

    if (selection_range.index == 0) {
        return nullptr;
    }

    //Remove the previous char.
    return CreateCommand(
        {},
        Range{ selection_range.index - 1, 1 }, 
        Range{ selection_range.index - 1, 0 });
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


void TextBoxEditor::HandleCut() {

    auto selection_range = Context().SelectionManager().SelectionRange();
    auto text = std::get<std::wstring_view>(Context().Core().GetText());

    //Copy the selected text to clipboard.
    auto selected_text = text.substr(selection_range.index, selection_range.length);
    clipboard::Clipboard::SetText(selected_text);

    //Remove the selected text.
    Context().Core().GetTextModel()->SetTextInRange({}, selection_range);
    Context().SelectionManager().SetSelectionRange(
        Range{ selection_range.index, 0 }, 
        text_box::SelectionOption::ScrollToCaret, 
        true);
}


void TextBoxEditor::HandlePaste() {

    try {

        auto text = zaf::clipboard::Clipboard::GetText();

        //Replace the current selection with the text in clipboard.
        auto selection_range = Context().SelectionManager().SelectionRange();
        Context().Core().GetTextModel()->SetTextInRange(text, selection_range);

        //Set the caret to the end of the pasted text.
        Range new_selection_range{ selection_range.index + text.length(), 0 };
        Context().SelectionManager().SetSelectionRange(
            new_selection_range,
            text_box::SelectionOption::ScrollToCaret | text_box::SelectionOption::SetCaretToEnd,
            true);
    }
    catch (const zaf::Error&) {

    }
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateCommand(
    std::wstring new_text,
    const Range& old_selection_range,
    const Range& new_selection_range) const {

    TextBoxEditCommand::EditInfo new_edit_info{
        std::move(new_text),
        new_selection_range,
        false,
    };

    auto old_caret_index = Context().SelectionManager().CaretIndex();
    auto old_text = Context().Core().GetTextModel()->GetText().substr(
        old_selection_range.index,
        old_selection_range.length);

    TextBoxEditCommand::EditInfo old_edit_info{
        std::wstring{ old_text },
        old_selection_range,
        old_caret_index == old_selection_range.index,
    };

    return std::make_unique<TextBoxEditCommand>(
        std::move(new_edit_info),
        std::move(old_edit_info));
}


void TextBoxEditor::ExecuteCommand(std::unique_ptr<TextBoxEditCommand> command) {

    command->Do(Context());

    edit_commands_.push_back(std::move(command));
    ++next_command_index_;

    edit_commands_.erase(edit_commands_.begin() + next_command_index_, edit_commands_.end());
}


void TextBoxEditor::HandleUndo() {

    if (next_command_index_ == 0) {
        return;
    }

    --next_command_index_;
    const auto& command = edit_commands_[next_command_index_];
    command->Undo(Context());
}

}