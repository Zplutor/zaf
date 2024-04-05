#include <zaf/internal/textual/text_box_keyboard_input_handler.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/control/text_box.h>
#include <zaf/internal/textual/text_box_editor.h>
#include <zaf/internal/textual/text_box_index_manager.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/input/keyboard.h>

namespace zaf::internal {

TextBoxKeyboardInputHandler::TextBoxKeyboardInputHandler(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxKeyboardInputHandler::Initialize() {

}


void TextBoxKeyboardInputHandler::HandleKeyDown(const KeyDownInfo& event_info) {

    auto virtual_key = event_info.Message().Key();
    if (virtual_key == Key::Left) {
        BackwardCaretIndex(Keyboard::IsShiftDown());
    }
    else if (virtual_key == Key::Right) {
        ForwardCaretIndex(Keyboard::IsShiftDown());
    }
    else if (virtual_key == Key::Up) {
        UpwardCaretIndex(Keyboard::IsShiftDown());
    }
    else if (virtual_key == Key::Down) {
        DownwardCaretIndex(Keyboard::IsShiftDown());
    }
    else if (virtual_key == Key::Home) {
        if (Keyboard::IsCtrlDown()) {
            MoveCaretIndexToTextHead();
        }
        else {
            MoveCaretIndexToLineHead();
        }
    }
    else if (virtual_key == Key::End) {
        if (Keyboard::IsCtrlDown()) {
            MoveCaretIndexToTextEnd();
        }
        else {
            MoveCaretIndexToLineEnd();
        }
    }
    else if (virtual_key == Key::C && Keyboard::IsCtrlDown()) {
        HandleCopy();
    }
    else if (virtual_key == Key::A && Keyboard::IsCtrlDown()) {
        HandleSelectAll();
    }
    else {
        Context().Editor().HandleKeyDown(event_info);
    }
}


void TextBoxKeyboardInputHandler::BackwardCaretIndex(bool expand_selection) {

    if (!expand_selection) {
        if (TryToMoveCaretToSelectionEdge(true)) {
            return;
        }
    }

    auto caret_index = Context().SelectionManager().CaretIndex();
    auto new_index = Context().IndexManager().GetPreviousIndex(caret_index);

    SetCaretIndexByKey(new_index, expand_selection, true);
}


void TextBoxKeyboardInputHandler::ForwardCaretIndex(bool expand_selection) {

    if (!expand_selection) {
        if (TryToMoveCaretToSelectionEdge(false)) {
            return;
        }
    }

    auto caret_index = Context().SelectionManager().CaretIndex();
    auto new_index = Context().IndexManager().GetNextIndex(caret_index);

    SetCaretIndexByKey(new_index, expand_selection, true);
}


bool TextBoxKeyboardInputHandler::TryToMoveCaretToSelectionEdge(bool backward) {

    //Move the caret index to the beginning or the end of the selection if its length is not zero.
    const auto& selection_range = Context().SelectionManager().SelectionRange();
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

    auto text_layout = Context().GetTextLayout();
    auto caret_hit_test_result = text_layout.HitTestIndex(
        Context().SelectionManager().CaretIndex(),
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
        Point{ Context().SelectionManager().CaretLastX(), previous_line_y});

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
    SetCaretIndexByKey(Context().TextModel().GetText().length(), Keyboard::IsShiftDown(), true);
}


std::size_t TextBoxKeyboardInputHandler::LocateCurrentLineEndIndex() const {

    auto line_info = LocateCurrentLineInfo();
    if (line_info.line_length == 0) {
        return line_info.line_char_index;
    }

    std::wstring_view text = Context().TextModel().GetText();

    auto end_index = line_info.line_char_index + line_info.line_length - 1;
    if (text[end_index] == L'\r') {
        return end_index;
    }

    if (text[end_index] == L'\n') {

        if (line_info.line_length == 1) {
            return end_index;
        }

        if (text[end_index - 1] == L'\r') {
            return end_index - 1;
        }
        return end_index;
    }

    return line_info.line_char_index;
}


TextBoxKeyboardInputHandler::LineInfo TextBoxKeyboardInputHandler::LocateCurrentLineInfo() const {

    auto line_metrics = Context().GetTextLayout().GetLineMetrics();
    auto caret_index = Context().SelectionManager().CaretIndex();

    LineInfo line_info;

    for (const auto& each_line : line_metrics) {

        line_info.line_length = each_line.Length();
        line_info.line_height = each_line.Height();

        auto line_end_index = line_info.line_char_index + each_line.Length();
        if (line_info.line_char_index <= caret_index && caret_index < line_end_index) {
            break;
        }

        line_info.line_y += each_line.Height();
        line_info.line_char_index = line_end_index;
    }

    return line_info;
}


void TextBoxKeyboardInputHandler::SetCaretIndexByKey(
    std::size_t new_index,
    bool expand_selection,
    bool update_caret_x) {

    auto& selection_manager = Context().SelectionManager();
    auto caret_index = (std::min)(new_index, Context().Owner().TextLength());

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
        clipboard::Clipboard::SetText(Context().Owner().SelectedText());
    }
    catch (const Error&) {

    }
}


void TextBoxKeyboardInputHandler::HandleSelectAll() {

    Context().SelectionManager().SetSelectionRange(
        Range{ 0, Context().TextModel().GetText().length() },
        textual::SelectionOption::SetCaretToEnd | textual::SelectionOption::ScrollToCaret, 
        std::nullopt,
        true);
}

}