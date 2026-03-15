#include <zaf/control/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/control/internal/textual/text_model.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/text_box.h>
#include <zaf/creation.h>
#include <zaf/control/internal/textual/text_box_caret_manager.h>
#include <zaf/control/internal/textual/text_box_clipboard_operation.h>
#include <zaf/control/internal/textual/text_box_editor.h>
#include <zaf/control/internal/textual/text_box_index_manager.h>
#include <zaf/control/internal/textual/text_box_selection_manager.h>
#include <zaf/input/keyboard.h>

namespace zaf::internal {

TextBoxKeyboardInputHandler::TextBoxKeyboardInputHandler(TextBox& owner) : owner_(owner) {

}


void TextBoxKeyboardInputHandler::Initialize() {

}


void TextBoxKeyboardInputHandler::HandleKeyDown(const KeyDownInfo& event_info) {

    auto key = event_info.Message().Key();
    if (TryToHandleCaretRelatedKeyEvent(key)) {
        return;
    }

    if (key == Key::C && Keyboard::IsCtrlDown()) {
        HandleCopy();
    }
    else if (key == Key::A && Keyboard::IsCtrlDown()) {
        HandleSelectAll();
    }
    else {
        owner_.Editor().HandleKeyDown(event_info);
    }
}


bool TextBoxKeyboardInputHandler::TryToHandleCaretRelatedKeyEvent(Key key) {

    //Ignore these key events if the caret is disabled.
    if (!owner_.CaretManager().IsCaretEnabled()) {
        return false;
    }

    if (key == Key::Left) {
        BackwardCaretIndex(Keyboard::IsShiftDown());
        return true;
    }

    if (key == Key::Right) {
        ForwardCaretIndex(Keyboard::IsShiftDown());
        return true;
    }

    if (key == Key::Up) {
        UpwardCaretIndex(Keyboard::IsShiftDown());
        return true;
    }

    if (key == Key::Down) {
        DownwardCaretIndex(Keyboard::IsShiftDown());
        return true;
    }

    if (key == Key::Home) {
        if (Keyboard::IsCtrlDown()) {
            MoveCaretIndexToTextHead();
        }
        else {
            MoveCaretIndexToLineHead();
        }
        return true;
    }

    if (key == Key::End) {
        if (Keyboard::IsCtrlDown()) {
            MoveCaretIndexToTextEnd();
        }
        else {
            MoveCaretIndexToLineEnd();
        }
        return true;
    }

    return false;
}


void TextBoxKeyboardInputHandler::BackwardCaretIndex(bool expand_selection) {

    if (!expand_selection) {
        if (TryToMoveCaretToSelectionEdge(true)) {
            return;
        }
    }

    auto caret_index = owner_.SelectionManager().CaretIndex();
    auto new_index = owner_.IndexManager().GetBackwardIndex(caret_index);

    SetCaretIndexByKey(new_index, expand_selection, true);
}


void TextBoxKeyboardInputHandler::ForwardCaretIndex(bool expand_selection) {

    if (!expand_selection) {
        if (TryToMoveCaretToSelectionEdge(false)) {
            return;
        }
    }

    auto caret_index = owner_.SelectionManager().CaretIndex();
    auto new_index = owner_.IndexManager().GetForwardIndex(caret_index);

    SetCaretIndexByKey(new_index, expand_selection, true);
}


bool TextBoxKeyboardInputHandler::TryToMoveCaretToSelectionEdge(bool backward) {

    //Move the caret index to the beginning or the end of the selection if its length is not zero.
    const auto& selection_range = owner_.SelectionManager().SelectionRange();
    if (selection_range.length == 0) {
        return false;
    }

    SetCaretIndexByKey(
        backward ? selection_range.index : selection_range.EndIndex(),
        false,
        true);

    return true;
}


void TextBoxKeyboardInputHandler::UpwardCaretIndex(bool expand_selection) {
    UpdateCaretIndexVertically(false, expand_selection);
}


void TextBoxKeyboardInputHandler::DownwardCaretIndex(bool expand_selection) {
    UpdateCaretIndexVertically(true, expand_selection);
}


void TextBoxKeyboardInputHandler::UpdateCaretIndexVertically(
    bool is_downward,
    bool expand_selection) {

    auto text_layout = owner_.InnerTextLayout();
    auto caret_hit_test_result = text_layout.HitTestIndex(
        owner_.SelectionManager().CaretIndex(),
        false);

    auto current_line_info = LocateCurrentLineInfo();
    auto previous_line_y = caret_hit_test_result.Metrics().Top();
    if (is_downward) {
        previous_line_y += current_line_info.line_height;
    }
    else {
        previous_line_y -= current_line_info.line_height;
    }
    previous_line_y = (std::max)(previous_line_y, 0.f);

    auto previous_line_hit_test_result = text_layout.HitTestPoint(
        Point{ owner_.SelectionManager().CaretLastX(), previous_line_y });

    auto new_index = previous_line_hit_test_result.Metrics().TextIndex();
    if (previous_line_hit_test_result.IsTrailingHit()) {
        ++new_index;
    }

    SetCaretIndexByKey(new_index, expand_selection, false);
}


void TextBoxKeyboardInputHandler::MoveCaretIndexToLineHead() {

    auto current_line_info = LocateCurrentLineInfo();
    SetCaretIndexByKey(current_line_info.line_char_index, Keyboard::IsShiftDown(), true);
}


void TextBoxKeyboardInputHandler::MoveCaretIndexToLineEnd() {

    auto line_end_index = LocateCurrentLineEndIndex();
    SetCaretIndexByKey(line_end_index, Keyboard::IsShiftDown(), true);
}


void TextBoxKeyboardInputHandler::MoveCaretIndexToTextHead() {
    SetCaretIndexByKey(0, Keyboard::IsShiftDown(), true);
}


void TextBoxKeyboardInputHandler::MoveCaretIndexToTextEnd() {
    SetCaretIndexByKey(owner_.InnerTextModel().Text().length(), Keyboard::IsShiftDown(), true);
}


std::size_t TextBoxKeyboardInputHandler::LocateCurrentLineEndIndex() const {

    auto line_info = LocateCurrentLineInfo();
    if (line_info.line_length == 0) {
        return line_info.line_char_index;
    }

    std::wstring_view text = owner_.InnerTextModel().Text();

    auto last_char_index = line_info.line_char_index + line_info.line_length - 1;
    if (text[last_char_index] == L'\r') {
        return last_char_index;
    }

    if (text[last_char_index] == L'\n') {

        if (line_info.line_length == 1) {
            return last_char_index;
        }

        if (text[last_char_index - 1] == L'\r') {
            return last_char_index - 1;
        }
        return last_char_index;
    }

    return line_info.line_char_index + line_info.line_length;
}


TextBoxKeyboardInputHandler::LineInfo TextBoxKeyboardInputHandler::LocateCurrentLineInfo() const {

    auto line_metrics = owner_.InnerTextLayout().GetLineMetrics();
    auto caret_index = owner_.SelectionManager().CaretIndex();

    LineInfo current_line_info;
    float last_line_y{};
    std::size_t last_line_end_index{};

    for (const auto& each_line : line_metrics) {

        current_line_info.line_char_index = last_line_end_index;
        current_line_info.line_length = each_line.Length();
        current_line_info.line_y = last_line_y;
        current_line_info.line_height = each_line.Height();

        auto line_end_index = current_line_info.line_char_index + each_line.Length();
        if (current_line_info.line_char_index <= caret_index && caret_index < line_end_index) {
            break;
        }

        last_line_y += each_line.Height();
        last_line_end_index = line_end_index;
    }

    return current_line_info;
}


void TextBoxKeyboardInputHandler::SetCaretIndexByKey(
    std::size_t new_index,
    bool expand_selection,
    bool update_caret_x) {

    auto& selection_manager = owner_.SelectionManager();
    auto caret_index = (std::min)(new_index, owner_.TextLength());

    std::size_t selection_begin = caret_index;
    std::size_t selection_end = caret_index;
    std::size_t anchor_index = caret_index;

    if (expand_selection) {
        anchor_index = selection_manager.AnchorIndex();
        selection_begin = (std::min)(caret_index, anchor_index);
        selection_end = (std::max)(caret_index, anchor_index);
    }

    auto new_selection_range = Range::FromIndexPair(selection_begin, selection_end);

    auto selection_option = textual::SelectionOption::ScrollToCaret;
    if (new_selection_range.index == caret_index) {
        selection_option |= textual::SelectionOption::SetCaretToBegin;
    }
    else {
        selection_option |= textual::SelectionOption::SetCaretToEnd;
    }

    selection_manager.SetSelectionRange(
        new_selection_range, 
        selection_option, 
        anchor_index,
        update_caret_x);
}


void TextBoxKeyboardInputHandler::HandleCopy() {
    try {
        PerformCopy();
    }
    catch (...) {

    }
}


bool TextBoxKeyboardInputHandler::PerformCopy() {

    auto selection_range = owner_.SelectionManager().SelectionRange();
    if (selection_range.IsEmpty()) {
        return false;
    }

    textual::CopyingInfo event_info{ 
        As<TextBox>(owner_.shared_from_this()),
        selection_range 
    };

    copying_event_.Raise(event_info);
    if (event_info.IsHandled()) {

        const auto& data_object = event_info.DataObject();
        if (!data_object) {
            return false;
        }

        clipboard::Clipboard::SetDataObject(data_object);
        return true;
    }

    auto selected_styled_text = owner_.InnerTextModel().StyledText().GetSubText(selection_range);
    internal::SaveStyledTextToClipboard(std::move(selected_styled_text));
    return true;
}


void TextBoxKeyboardInputHandler::HandleSelectAll() {

    owner_.SelectionManager().SetSelectionRange(
        Range{ 0, owner_.InnerTextModel().Text().length() },
        textual::SelectionOption::SetCaretToEnd | textual::SelectionOption::ScrollToCaret, 
        std::nullopt,
        true);
}

}
