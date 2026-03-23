#include <zaf/control/internal/textual/text_box_editor.h>
#include <cwctype>
#include <zaf/base/auto_reset.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/event/ime_event_infos.h>
#include <zaf/control/internal/textual/text_model.h>
#include <zaf/control/internal/textual/text_box_clipboard_operation.h>
#include <zaf/control/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/control/internal/textual/text_box_index_manager.h>
#include <zaf/control/internal/textual/text_box_selection_manager.h>
#include <zaf/input/keyboard.h>
#include <zaf/window/input_method_context.h>

using namespace zaf::textual;

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

TextBoxEditor::TextBoxEditor(TextBox& owner) : owner_(owner) {

}


void TextBoxEditor::Initialize() {

    Disposables() += owner_.InnerTextModel().TextChangedEvent().Subscribe(
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
            auto auto_reset = MakeAutoReset(is_performing_edit_, true);
            auto command = HandleKey(key);
            if (command) {
                ExecuteNormalCommand(std::move(command));
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

    if (owner_.InnerTextModel().IsMultiline()) {
        InnerPerformInputText(L"\r\n", false);
    }
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleDelete() {

    const auto& selection_range = owner_.SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateNormalCommand({}, selection_range, true);
    }

    auto next_index = owner_.IndexManager().GetForwardIndex(selection_range.index);
    if (next_index == selection_range.index) {
        //Index not changed, no action.
        return nullptr;
    }

    //Remove chars between next index and current selection index.
    return CreateNormalCommand(
        {},
        Range::FromIndexPair(selection_range.index, next_index),
        true);
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBatchDelete() {

    const auto& selection_range = owner_.SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateNormalCommand({}, selection_range, true);
    }

    //Determine the word range.
    auto text = owner_.InnerTextModel().Text();
    auto word_range = owner_.WordExtractor()(text, selection_range.index);

    //Nothing can be removed.
    if (word_range.EndIndex() <= selection_range.index) {
        return nullptr;
    }

    return CreateNormalCommand(
        {},
        Range{ selection_range.index, word_range.EndIndex() - selection_range.index },
        true);
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBatchBackspace() {

    const auto& selection_range = owner_.SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateNormalCommand({}, selection_range, true);
    }

    //Determine the word range. Note that the index used to determine should be prior to the caret 
    //index.
    auto text = owner_.InnerTextModel().Text();

    auto determined_index = selection_range.index > 0 ? selection_range.index - 1 : 0;
    auto word_range = owner_.WordExtractor()(text, determined_index);

    //Nothing can be removed.
    if (word_range.index >= selection_range.index) {
        return nullptr;
    }

    //Remove text in the word ahead of the caret.
    return CreateNormalCommand(
        {},
        Range{ word_range.index, selection_range.index - word_range.index },
        true);
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::HandleBackspace() {

    const auto& selection_range = owner_.SelectionManager().SelectionRange();

    //Remove the selected text.
    if (selection_range.length > 0) {
        return CreateNormalCommand({}, selection_range, true);
    }

    auto previous_index = owner_.IndexManager().GetBackwardIndex(selection_range.index);
    if (previous_index == selection_range.index) {
        //Index not changed, no action.
        return nullptr;
    }

    //Remove chars between previous index and current selection index.
    return CreateNormalCommand(
        {},
        Range::FromIndexPair(previous_index, selection_range.index),
        true);
}


bool TextBoxEditor::PerformCut() {

    if (!CanEdit()) {
        return false;
    }

    auto auto_reset = MakeAutoReset(is_performing_edit_, true);

    if (!owner_.KeyboardInputHandler().PerformCopy()) {
        return false;
    }

    //Remove the selected text.
    auto selection_range = owner_.SelectionManager().SelectionRange();
    auto command = CreateNormalCommand({}, selection_range, true);
    ExecuteNormalCommand(std::move(command));
    return true;
}


bool TextBoxEditor::PerformPaste() {

    if (!CanEdit()) {
        return false;
    }

    auto data_object = clipboard::Clipboard::GetDataObject();

    textual::PastingInfo event_info{
        As<TextBox>(owner_.shared_from_this()),
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
        const auto& default_style = owner_.InnerTextModel().StyledText().DefaultStyle();
        styled_text.SetDefaultStyle(default_style);
    }

    return InputStyledText(std::move(styled_text), true);
}


void TextBoxEditor::HandleCharInput(const CharInputInfo& event_info) {

    auto ch = event_info.Message().Char();
    if (ShouldIgnoreChar(ch)) {
        return;
    }

    InnerPerformInputText(std::wstring_view(&ch, 1), false);
}


bool TextBoxEditor::PerformInputText(std::wstring_view text) {
    return InnerPerformInputText(text, true);
}


bool TextBoxEditor::InnerPerformInputText(std::wstring_view text, bool can_truncate) {

    if (!CanEdit()) {
        return false;
    }

    StyledText styled_text;
    styled_text.SetText(std::wstring{ text });

    FillTextStyleFromSelection(styled_text);
    return InputStyledText(std::move(styled_text), can_truncate);
}


void TextBoxEditor::FillTextStyleFromSelection(StyledText& styled_text) const {

    const auto& current_styled_text = owner_.InnerTextModel().StyledText();
    std::size_t index_to_inherit{};

    const auto& selection_manager = owner_.SelectionManager();
    auto selection_range = selection_manager.SelectionRange();
    auto caret_index = selection_manager.CaretIndex();

    if (selection_range.IsEmpty()) {
        
        if (caret_index > 0) {
            index_to_inherit = caret_index - 1;
        }
        else {
            index_to_inherit = caret_index;
        }
    }
    else {

        if (caret_index == selection_range.index) {
            index_to_inherit = selection_range.index;
        }
        else {
            index_to_inherit = selection_range.EndIndex() - 1;
        }
    }

    styled_text.SetDefaultFont(current_styled_text.GetFontAtIndex(index_to_inherit));
    styled_text.SetDefaultTextColor(current_styled_text.GetTextColorAtIndex(index_to_inherit));
    styled_text.SetDefaultTextBackColor(
        current_styled_text.GetTextBackColorAtIndex(index_to_inherit));
}


bool TextBoxEditor::PerformInputInlineObject(std::shared_ptr<InlineObject> inline_object) {

    ZAF_EXPECT(inline_object);

    if (!CanEdit()) {
        return false;
    }

    StyledText styled_text{ std::wstring(1, ObjectReplacementChar) };
    styled_text.AttachInlineObjectToRange(std::move(inline_object), Range{ 0, 1 });

    FillTextStyleFromSelection(styled_text);
    return InputStyledText(std::move(styled_text), false);
}


bool TextBoxEditor::PerformInputStyledText(StyledText styled_text) {
    return InnerPerformInputStyledText(std::move(styled_text), true);
}


bool TextBoxEditor::InnerPerformInputStyledText(StyledText styled_text, bool can_truncate) {

    if (!CanEdit()) {
        return false;
    }

    return InputStyledText(std::move(styled_text), can_truncate);
}


bool TextBoxEditor::InputStyledText(textual::StyledText styled_text, bool can_truncate) {

    auto selection_range = owner_.SelectionManager().SelectionRange();

    if (!EnforceMaxLength(styled_text, selection_range, can_truncate)) {
        return false;
    }

    if (styled_text.Text().empty()) {
        return false;
    }

    auto command = CreateNormalCommand(std::move(styled_text), selection_range, false);

    auto auto_reset = MakeAutoReset(is_performing_edit_, true);
    ExecuteNormalCommand(std::move(command));
    return true;
}


bool TextBoxEditor::EnforceMaxLength(
    textual::StyledText& styled_text,
    const Range& selection_range, 
    bool can_truncate) const {

    if (!max_length_.has_value()) {
        return true;
    }

    auto current_length = owner_.InnerTextModel().Text().length();
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


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::CreateNormalCommand(
    textual::StyledText new_text,
    const Range& replaced_selection_range,
    bool set_caret_to_begin) const {

    auto& selection_manager = owner_.SelectionManager();
    auto old_caret_index = selection_manager.CaretIndex();
    auto old_selection_range = selection_manager.SelectionRange();

    TextBoxEditCommand::SelectionInfo undo_selection_info;
    undo_selection_info.set_caret_to_begin = old_caret_index == replaced_selection_range.index;
    if (!old_selection_range.IsEmpty() && old_selection_range == replaced_selection_range) {
        undo_selection_info.select_slice = true;
    }

    TextBoxEditCommand::SelectionInfo do_selection_info{
        .set_caret_to_begin = set_caret_to_begin,
        .select_slice = false,
    };

    TextBoxEditCommand::EditParams edit_params{
        .replaced_range = replaced_selection_range,
        .styled_text_slice = std::move(new_text),
    };

    return std::make_unique<TextBoxEditCommand>(
        std::move(edit_params), 
        do_selection_info,
        undo_selection_info);
}


void TextBoxEditor::ExecuteNormalCommand(std::unique_ptr<TextBoxEditCommand> command) {

    // Cancel the current composition if there is any, otherwise the composition will be messed up
    // after the command is executed.
    CancelIMEComposition();

    command->Do(owner_);
    AddCommandToUndoHistory(std::move(command));
}


void TextBoxEditor::AddCommandToUndoHistory(std::unique_ptr<TextBoxEditCommand> command) {

    edit_commands_.erase(edit_commands_.begin() + next_command_index_, edit_commands_.end());

    if (AllowUndo()) {
        edit_commands_.push_back(std::move(command));
        ++next_command_index_;
    }
}


bool TextBoxEditor::PerformUndo() {

    if (!CanUndo()) {
        return false;
    }

    auto auto_reset = MakeAutoReset(is_performing_edit_, true);

    --next_command_index_;
    const auto& command = edit_commands_[next_command_index_];
    command->Undo(owner_);
    return true;
}


bool TextBoxEditor::PerformRedo() {

    if (!CanRedo()) {
        return false;
    }

    auto auto_reset = MakeAutoReset(is_performing_edit_, true);

    const auto& command = edit_commands_[next_command_index_];
    command->Do(owner_);
    ++next_command_index_;
    return true;
}


void TextBoxEditor::HandleIMEStartComposition(const IMEStartCompositionInfo& event_info) {

    composition_state_.reset();
    event_info.MarkAsHandled();

    if (!CanEdit()) {
        return;
    }

    composition_state_.emplace();
    RecordNewCompositionState();
}


void TextBoxEditor::HandleIMEComposition(const IMECompositionInfo& event_info) {

    event_info.MarkAsHandled();

    if (!CanEdit()) {
        return;
    }

    if (!composition_state_) {
        return;
    }

    auto context = InputMethodContext::FromWindowHandle(event_info.Message().WindowHandle());
    if (event_info.Message().HasResultString()) {
        auto result_string = context.GetResultString();
        CommitCompositionText(result_string);
    }

    if (event_info.Message().HasCompositionString()) {
        auto composition_string = context.GetCompositionString();
        InputCompositionText(std::move(composition_string));
    }

    if (event_info.Message().HasCaretPosition()) {

        auto caret_position = context.GetCaretPosition();
        Range new_selection_range{ 
            composition_state_->composition_range.index + caret_position, 
            0 
        };

        owner_.SelectionManager().SetSelectionRange(
            new_selection_range, 
            textual::SelectionOption::ScrollToCaret,
            std::nullopt,
            false);
    }
}


void TextBoxEditor::HandleIMEEndComposition(const IMEEndCompositionInfo& event_info) {

    ClearCompositionState();
    event_info.MarkAsHandled();
}


void TextBoxEditor::RecordNewCompositionState() {

    const auto& selection_manager = owner_.SelectionManager();
    composition_state_->original_caret_index = selection_manager.CaretIndex();

    const auto& selection_range = selection_manager.SelectionRange();
    composition_state_->original_selection_range = selection_range;
    composition_state_->composition_range = selection_range;
    composition_state_->original_text =
        owner_.TextModel().StyledText().GetSubText(selection_range);

    composition_state_->composition_command.reset();
}


void TextBoxEditor::InputCompositionText(std::wstring text) {
    auto new_command = ExecuteCompositionTextCommand(std::move(text));
    composition_state_->composition_command = std::move(new_command);
}


void TextBoxEditor::CommitCompositionText(std::wstring text) {
    auto new_command = ExecuteCompositionTextCommand(std::move(text));
    AddCommandToUndoHistory(std::move(new_command));
    RecordNewCompositionState();
}


std::unique_ptr<TextBoxEditCommand> TextBoxEditor::ExecuteCompositionTextCommand(
    std::wstring text) {

    auto new_text_length = text.length();

    StyledText new_styled_text{ std::move(text) };
    new_styled_text.SetDefaultFont(owner_.Font());

    TextBoxEditCommand::EditParams edit_params{
        .replaced_range = composition_state_->composition_range,
        .styled_text_slice = std::move(new_styled_text),
    };

    TextBoxEditCommand::SelectionInfo undo_selection_info;
    undo_selection_info.select_slice = true;
    if (composition_state_->original_caret_index ==
        composition_state_->original_selection_range.index) {
        undo_selection_info.set_caret_to_begin = true;
    }

    auto new_command = std::make_unique<TextBoxEditCommand>(
        std::move(edit_params),
        TextBoxEditCommand::SelectionInfo{},
        undo_selection_info);

    auto auto_reset = MakeAutoReset(is_performing_edit_, true);
    new_command->Do(owner_, composition_state_->original_text.Clone());

    composition_state_->composition_range.length = new_text_length;
    return new_command;
}


void TextBoxEditor::RetainCompositionText() {

    if (!composition_state_->composition_command) {
        return;
    }

    AddCommandToUndoHistory(std::move(composition_state_->composition_command));
    RecordNewCompositionState();
}


void TextBoxEditor::DropCompositionText() {

    // No need to process if there is no composition text.
    if (composition_state_->composition_range.IsEmpty()) {
        return;
    }

    // Replace the current composition text with an empty text, which effectively drops the 
    // composition text.
    auto new_command = ExecuteCompositionTextCommand({});

    // Only add the command to undo history if the composition text is not empty. Otherwise, the
    // command will be a no-op and should not be added to the undo history.
    if (!new_command->GetEditParams().styled_text_slice.Text().empty()) {
        AddCommandToUndoHistory(std::move(new_command));
    }
}


void TextBoxEditor::ClearCompositionState() {

    if (!composition_state_) {
        return;
    }

    DropCompositionText();
    composition_state_.reset();
}


void TextBoxEditor::CancelIMEComposition() {

    if (!composition_state_) {
        return;
    }

    if (CanEdit()) {
        RetainCompositionText();
    }

    auto window = owner_.Window();
    if (window) {
        auto context = InputMethodContext::FromWindow(*window);
        ImmNotifyIME(context.Handle(), NI_COMPOSITIONSTR, CPS_CANCEL, 0);
    }

    ClearCompositionState();
}


Range TextBoxEditor::GetCompositionRange() const noexcept {
    if (!composition_state_) {
        return {};
    }
    return composition_state_->composition_range;
}


void TextBoxEditor::OnTextModelChanged() {

    if (is_performing_edit_) {
        return;
    }

    CancelIMEComposition();

    //If the text is changed by other ways outside the editor, such as SetText() is called, we have
    //to clear the command queue.
    ClearCommands();
}


void TextBoxEditor::ClearCommands() {
    edit_commands_.clear();
    next_command_index_ = 0;
}

}
