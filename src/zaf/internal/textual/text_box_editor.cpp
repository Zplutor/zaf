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


bool TextBoxEditor::CanUndo() const noexcept {
    return next_command_index_ > 0;
}


bool TextBoxEditor::CanRedo() const noexcept {
    return next_command_index_ < edit_commands_.size();
}


void TextBoxEditor::HandleKeyDown(const KeyDownInfo& event_info) {

    try {
        auto key = event_info.Message().Key();
        if (key == Key::Enter) {
            HandleEnter();
        }
        else if ((key == Key::X) && Keyboard::IsCtrlDown()) {
            PerformCut();
        }
        else if ((key == Key::V) && Keyboard::IsCtrlDown()) {
            PerformPaste();
        }
        else if ((key == Key::Z) && Keyboard::IsCtrlDown()) {
            PerformUndo();
        }
        else if ((key == Key::Y) && Keyboard::IsCtrlDown()) {
            PerformRedo();
        }
        else {
            if (!CanEdit()) {
                return;
            }
            auto auto_reset = MakeAutoReset(is_editing_, true);
            auto command = HandleKey(key);
            if (command) {
                ExecuteCommand(std::move(command));
            }
        }
    }
    catch (...) {

    }
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleKey(Key key) {

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

    return nullptr;
}


void TextBoxEditor::HandleEnter() {

    if (Context().TextModel().IsMultiline()) {
        InnerPerformInputText(L"\r\n", false);
    }
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleDelete() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, true);
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
        true);
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBatchDelete() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, true);
    }

    //Determine the word range.
    auto text = Context().TextModel().Text();
    auto word_range = Context().Owner().WordExtractor()(text, selection_range.index);

    //Nothing can be removed.
    if (word_range.EndIndex() <= selection_range.index) {
        return nullptr;
    }

    return CreateCommand(
        {},
        Range{ selection_range.index, word_range.EndIndex() - selection_range.index },
        true);
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBatchBackspace() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, true);
    }

    //Determine the word range. Note that the index used to determine should be prior to the caret 
    //index.
    auto text = Context().TextModel().Text();

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
        true);
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBackspace() {

    const auto& selection_range = Context().SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateCommand({}, selection_range, true);
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
        true);
}


bool TextBoxEditor::PerformCut() {

    if (!CanEdit()) {
        return false;
    }

    auto auto_reset = MakeAutoReset(is_editing_, true);

    if (!Context().KeyboardInputHandler().PerformCopy()) {
        return false;
    }

    //Remove the selected text.
    auto selection_range = Context().SelectionManager().SelectionRange();
    auto command = CreateCommand({}, selection_range, true);
    ExecuteCommand(std::move(command));
    return true;
}


bool TextBoxEditor::PerformPaste() {

    if (!CanEdit()) {
        return false;
    }

    auto data_object = clipboard::Clipboard::GetDataObject();

    textual::PastingInfo event_info{
        As<TextBox>(Context().Owner().shared_from_this()),
        data_object
    };

    pasting_event_.Raise(event_info);
    if (event_info.IsHandled()) {
        return true;
    }

    textual::StyledText styled_text;
    bool is_styled_text{};
    bool is_loaded = LoadStyledTextFromClipboard(data_object, styled_text, is_styled_text);
    if (!is_loaded) {
        return false;
    }

    if (!is_styled_text) {
        const auto& default_style = Context().TextModel().StyledText().DefaultStyle();
        styled_text.SetDefaultStyle(default_style);
    }

    return InputStyledText(std::move(styled_text), true);
}


void TextBoxEditor::HandleCharInput(const CharInputInfo& event_info) {

    auto ch = event_info.Message().Char();
    if (ShouldIgnoreChar(ch)) {
        return;
    }

    InnerPerformInputText(std::wstring(1, ch), false);
}


bool TextBoxEditor::PerformInputText(std::wstring_view text) {
    return InnerPerformInputText(std::wstring{ text }, true);
}


bool TextBoxEditor::PerformInputStyledText(textual::StyledText styled_text) {
    
    if (!CanEdit()) {
        return false;
    }

    return InputStyledText(std::move(styled_text), true);
}


bool TextBoxEditor::InnerPerformInputText(std::wstring text, bool can_truncate) {

    if (!CanEdit()) {
        return false;
    }

    return InputStyledText(textual::StyledText{ std::move(text) }, can_truncate);
}


bool TextBoxEditor::InputStyledText(textual::StyledText styled_text, bool can_truncate) {

    auto selection_range = Context().SelectionManager().SelectionRange();

    if (!EnforceMaxLength(styled_text, selection_range, can_truncate)) {
        return false;
    }

    if (styled_text.Text().empty()) {
        return false;
    }

    auto command = CreateCommand(std::move(styled_text), selection_range, false);

    auto auto_reset = MakeAutoReset(is_editing_, true);
    ExecuteCommand(std::move(command));
    return true;
}


bool TextBoxEditor::EnforceMaxLength(
    textual::StyledText& styled_text,
    const Range& selection_range, 
    bool can_truncate) const {

    if (!max_length_.has_value()) {
        return true;
    }

    auto current_length = Context().TextModel().Text().length();
    auto new_length = current_length - selection_range.length + styled_text.Length();
    if (new_length <= *max_length_) {
        return true;
    }

    if (!can_truncate) {
        return false;
    }

    auto truncated_length = *max_length_ - current_length + selection_range.length;

    //Do not break the \r\n sequence.
    if (styled_text.Text()[truncated_length] == L'\n' && truncated_length > 0) {
        if (styled_text.Text()[truncated_length - 1] == L'\r') {
            --truncated_length;
        }
    }

    styled_text.Truncate(truncated_length);
    return true;
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateCommand(
    textual::StyledText new_text,
    const Range& replaced_selection_range,
    bool set_caret_to_begin) const {

    auto& selection_manager = Context().SelectionManager();
    auto old_caret_index = selection_manager.CaretIndex();
    auto old_selection_range = selection_manager.SelectionRange();

    const auto& styled_text = Context().TextModel().StyledText();
    auto old_text = styled_text.GetSubText(replaced_selection_range);

    TextBoxEditCommand::EditInfo undo_info{
        .replaced_range = Range{ replaced_selection_range.index, new_text.Length() },
        .styled_text_slice = std::move(old_text),
        .set_caret_to_begin = old_caret_index == old_selection_range.index,
        .select_slice = true,
    };

    TextBoxEditCommand::EditInfo do_info{
        .replaced_range = replaced_selection_range,
        .styled_text_slice = std::move(new_text),
        .set_caret_to_begin = set_caret_to_begin,
        .select_slice = false,
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


bool TextBoxEditor::PerformUndo() {

    if (!CanUndo()) {
        return false;
    }

    auto auto_reset = MakeAutoReset(is_editing_, true);

    --next_command_index_;
    const auto& command = edit_commands_[next_command_index_];
    command->Undo(Context());
    return true;
}


bool TextBoxEditor::PerformRedo() {

    if (!CanRedo()) {
        return false;
    }

    auto auto_reset = MakeAutoReset(is_editing_, true);

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