#include <zaf/internal/textual/text_box_editor.h>
#include <cwctype>
#include <zaf/base/auto_reset.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/internal/textual/text_box_clipboard_operation.h>
#include <zaf/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/internal/textual/text_box_index_manager.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/input/keyboard.h>

namespace zaf::internal {
namespace {

bool ShouldIgnoreChar(wchar_t ch) {

    //Some control chars in ASCII.
    //Note: control chars are typically handled in key down event rather than char input event.
    switch (ch) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:  //Backspace
    case 10: //LF
    case 11: //VerticalTab. May be supported if vertical text layout is supported.
    case 12:
    case 13: //CR
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
    case 127: //DEL
        return true;
    default:
        return false;
    }
}

}

TextBoxEditor::TextBoxEditor(TextBoxModuleContext* context) : TextBoxModule(context) {

}


void TextBoxEditor::Initialize() {

    Subscriptions() += Context().TextModel().TextChangedEvent().Subscribe(
        std::bind(&TextBoxEditor::OnTextModelChanged, this));
}


void TextBoxEditor::SetCanEdit(bool can_edit) {

    if (can_edit_ == can_edit) {
        return;
    }

    can_edit_ = can_edit;

    //Clear all commands if it cannot edit.
    if (!can_edit_) {
        ClearCommands();
    }

    can_edit_changed_event_.Raise(can_edit_);
}


void TextBoxEditor::SetAllowUndo(bool allow_undo) {

    if (allow_undo_ == allow_undo) {
        return;
    }

    allow_undo_ = allow_undo;

    //Clear all commands if undo is now allowed.
    if (!allow_undo_) {
        ClearCommands();
    }
}


bool TextBoxEditor::CanUndo() const {
    return next_command_index_ > 0;
}


bool TextBoxEditor::Undo() {
    auto auto_reset = MakeAutoReset(is_editing_, true);
    return HandleUndo();
}


bool TextBoxEditor::CanRedo() const {
    return next_command_index_ < edit_commands_.size();
}


bool TextBoxEditor::Redo() {
    auto auto_reset = MakeAutoReset(is_editing_, true);
    return HandleRedo();
}


void TextBoxEditor::HandleKeyDown(const KeyDownInfo& event_info) {

    if (!can_edit_) {
        return;
    }

    auto auto_reset = MakeAutoReset(is_editing_, true);

    auto key = event_info.Message().Key();
    auto command = HandleKey(key);
    if (!command) {
        return;
    }

    ExecuteCommand(std::move(command));
}


void TextBoxEditor::HandleCharInput(const CharInputInfo& event_info) {

    if (!can_edit_) {
        return;
    }

    auto auto_reset = MakeAutoReset(is_editing_, true);

    auto ch = event_info.Message().Char();
    auto command = HandleChar(ch);
    if (!command) {
        return;
    }

    ExecuteCommand(std::move(command));
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleKey(Key key) {

    if (key == Key::Enter) {
        return HandleEnter();
    }

    if (key == Key::Delete) {
        if (Keyboard::IsCtrlDown()) {
            return HandleBatchDelete();
        }
        return HandleDelete();
    }

    if (key == Key::Backspace) {
        if (Keyboard::IsCtrlDown()) {
            return HandleBatchBackspace();
        }
        return HandleBackspace();
    }

    if ((key == Key::X) && Keyboard::IsCtrlDown()) {
        return HandleCut();
    }

    if ((key == Key::V) && Keyboard::IsCtrlDown()) {
        return HandlePaste();
    }

    if ((key == Key::Z) && Keyboard::IsCtrlDown()) {
        HandleUndo();
        return nullptr;
    }

    if ((key == Key::Y) && Keyboard::IsCtrlDown()) {
        HandleRedo();
        return nullptr;
    }

    return nullptr;
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleEnter() {
    return CreateInsertTextCommand(L"\r\n");
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleDelete() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, Range{ selection_range.index, 0 });
    }

    auto next_index = Context().IndexManager().GetForwardIndex(selection_range.index);
    if (next_index == selection_range.index) {
        //Index not changed, no action.
        return nullptr;
    }

    //Remove chars between next index and current selection index.
    return CreateCommand(
        {},
        Range::FromIndexPair(selection_range.index, next_index),
        Range{ selection_range.index, 0 });
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBatchDelete() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, Range{ selection_range.index, 0 });
    }

    //Determine the word range.
    auto text = Context().TextModel().GetText();
    auto word_range = Context().Owner().WordExtractor()(text, selection_range.index);

    //Nothing can be removed.
    if (word_range.EndIndex() <= selection_range.index) {
        return nullptr;
    }

    return CreateCommand(
        {},
        Range{ selection_range.index, word_range.EndIndex() - selection_range.index },
        Range{ selection_range.index, 0 });
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBatchBackspace() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, Range{ selection_range.index, 0 });
    }

    //Determine the word range. Note that the index used to determine should be prior to the caret 
    //index.
    auto text = Context().TextModel().GetText();

    auto determined_index = selection_range.index > 0 ? selection_range.index - 1 : 0;
    auto word_range = Context().Owner().WordExtractor()(text, determined_index);

    //Nothing can be removed.
    if (word_range.index >= selection_range.index) {
        return nullptr;
    }

    //Remove text in the word ahead of the caret.
    return CreateCommand(
        {},
        Range{ word_range.index, selection_range.index - word_range.index },
        Range{ word_range.index, 0 });
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleCut() {

    Context().KeyboardInputHandler().HandleCopy();

    //Remove the selected text.
    auto selection_range = Context().SelectionManager().SelectionRange();
    return CreateCommand({}, selection_range, Range{ selection_range.index, 0 });
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandlePaste() {

    try {

        auto text = zaf::clipboard::Clipboard::GetText();

        //Replace the current selection with the text in clipboard.
        auto selection_range = Context().SelectionManager().SelectionRange();
        Range new_selection_range{ selection_range.index + text.length(), 0 };
        return CreateCommand(text, selection_range, new_selection_range);
    }
    catch (...) {
        return nullptr;
    }
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleChar(wchar_t ch) {

    //Ignore specific control chars.
    if (ShouldIgnoreChar(ch)) {
        return nullptr;
    }

    return CreateInsertTextCommand(std::wstring(1, ch));
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBackspace() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, Range{ selection_range.index, 0 });
    }

    auto previous_index = Context().IndexManager().GetBackwardIndex(selection_range.index);
    if (previous_index == selection_range.index) {
        //Index not changed, no action.
        return nullptr;
    }

    //Remove chars between previous index and current selection index.
    return CreateCommand(
        {},
        Range::FromIndexPair(previous_index, selection_range.index),
        Range{ previous_index, 0 });
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateInsertTextCommand(std::wstring new_text) {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    return CreateCommand(
        std::move(new_text),
        selection_range,
        Range{ selection_range.index + new_text.length(), 0});
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateCommand(
    std::wstring new_text,
    const Range& replaced_selection_range,
    const Range& new_selection_range) const {

    auto& selection_manager = Context().SelectionManager();
    auto old_caret_index = selection_manager.CaretIndex();
    auto old_selection_range = selection_manager.SelectionRange();

    auto old_text = Context().TextModel().StyledText().Slice(replaced_selection_range);

    TextBoxEditCommand::EditInfo undo_info{
        Range{ replaced_selection_range.index, new_text.length() },
        std::move(old_text),
        old_selection_range,
        old_caret_index == old_selection_range.index,
    };

    TextBoxEditCommand::EditInfo do_info{
        replaced_selection_range,
        textual::StyledTextSlice{ replaced_selection_range.index, std::move(new_text), {} },
        new_selection_range,
        false,
    };

    return std::make_unique<TextBoxEditCommand>(std::move(do_info), std::move(undo_info));
}


void TextBoxEditor::ExecuteCommand(std::unique_ptr<TextBoxEditCommand> command) {

    edit_commands_.erase(edit_commands_.begin() + next_command_index_, edit_commands_.end());

    command->Do(Context());

    if (AllowUndo()) {
        edit_commands_.push_back(std::move(command));
        ++next_command_index_;
    }
}


bool TextBoxEditor::HandleUndo() {

    if (!CanUndo()) {
        return false;
    }

    --next_command_index_;
    const auto& command = edit_commands_[next_command_index_];
    command->Undo(Context());
    return true;
}


bool TextBoxEditor::HandleRedo() {

    if (!CanRedo()) {
        return false;
    }

    const auto& command = edit_commands_[next_command_index_];
    command->Do(Context());
    ++next_command_index_;
    return true;
}


void TextBoxEditor::OnTextModelChanged() {

    if (is_editing_) {
        return;
    }

    //If the text is changed by other ways outside the editor, such as SetText() is called, we have
    //to clear the command queue.
    ClearCommands();
}


void TextBoxEditor::ClearCommands() {
    edit_commands_.clear();
    next_command_index_ = 0;
}

}