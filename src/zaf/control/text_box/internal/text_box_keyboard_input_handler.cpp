#include <zaf/control/text_box/internal/text_box_keyboard_input_handler.h>
#include <zaf/clipboard/clipboard.h>
#include <zaf/control/text_box.h>
#include <zaf/control/text_box/internal/text_box_core.h>
#include <zaf/control/text_box/internal/text_box_editor.h>
#include <zaf/control/text_box/internal/text_box_module_context.h>
#include <zaf/control/text_box/internal/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxKeyboardInputHandler::TextBoxKeyboardInputHandler(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxKeyboardInputHandler::HandleKeyDown(const KeyDownInfo& event_info) {

    constexpr auto is_shift_pressed = []() {
        return !!(GetKeyState(VK_SHIFT) >> 15);
    };

    auto virtual_key = event_info.Message().VirtualKey();
    if (virtual_key == VK_LEFT) {
        BackwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == VK_RIGHT) {
        ForwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == VK_UP) {
        UpwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == VK_DOWN) {
        DownwardCaretIndex(is_shift_pressed());
    }
    else if (virtual_key == L'C') {
        if (GetKeyState(VK_CONTROL) >> 15) {
            HandleCopy();
        }
    }
    else if (virtual_key == L'A') {
        if (GetKeyState(VK_CONTROL) >> 15) {
            HandleSelectAll();
        }
    }
    else {
        Context().Editor().HandleKeyDown(event_info);
    }
}


void TextBoxKeyboardInputHandler::BackwardCaretIndex(bool expand_selection) {

    std::size_t new_index = Context().SelectionManager().CaretIndex();
    if (new_index > 0) {

        --new_index;

        if (new_index > 0) {

            //Skip CRLF line break.
            auto text = std::get<std::wstring_view>(Context().Core().GetText());
            if (text[new_index - 1] == L'\r' && text[new_index] == L'\n') {

                --new_index;
            }
        }
    }

    SetCaretIndexByKey(new_index, expand_selection, true);
}


void TextBoxKeyboardInputHandler::ForwardCaretIndex(bool expand_selection) {

    auto text = std::get<std::wstring_view>(Context().Core().GetText());

    auto caret_index = Context().SelectionManager().CaretIndex();
    std::size_t new_index = caret_index;
    if (new_index < text.length()) {

        ++new_index;

        if (new_index < text.length()) {

            //Skip CRLF line break.
            if (text[caret_index] == L'\r' && text[new_index] == L'\n') {
                ++new_index;
            }
        }
    }

    SetCaretIndexByKey(new_index, expand_selection, true);
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

    auto text_layout = Context().Core().GetTextLayout();
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


TextBoxKeyboardInputHandler::LineInfo TextBoxKeyboardInputHandler::LocateCurrentLineInfo() {

    auto line_metrics = Context().Core().GetTextLayout().GetLineMetrics();
    auto caret_index = Context().SelectionManager().CaretIndex();

    LineInfo line_info;

    for (const auto& each_line : line_metrics) {

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

    auto old_index = Context().SelectionManager().CaretIndex();
    auto caret_index = (std::min)(new_index, Context().Core().GetTextLength());

    std::size_t selection_begin = caret_index;
    std::size_t selection_end = caret_index;
    if (expand_selection) {

        auto selection_range = Context().SelectionManager().SelectionRange();
        if (old_index == selection_range.Index()) {

            selection_begin = (std::min)(caret_index, selection_range.EndIndex());
            selection_end = (std::max)(caret_index, selection_range.EndIndex());
        }
        else if (old_index == selection_range.EndIndex()) {

            selection_begin = (std::min)(selection_range.Index(), caret_index);
            selection_end = (std::max)(selection_range.Index(), caret_index);
        }
    }

    auto new_selection_range = Range::FromIndexPair(selection_begin, selection_end);

    auto selection_option = text_box::SelectionOption::ScrollToCaret;
    if (new_selection_range.index == caret_index) {
        selection_option |= text_box::SelectionOption::SetCaretToBegin;
    }
    else {
        selection_option |= text_box::SelectionOption::SetCaretToEnd;
    }

    Context().SelectionManager().SetSelectionRange(
        new_selection_range, 
        selection_option, 
        update_caret_x);
}


void TextBoxKeyboardInputHandler::HandleCopy() {
    clipboard::Clipboard::SetText(Context().Owner().SelectedText());
}


void TextBoxKeyboardInputHandler::HandleSelectAll() {

    Context().SelectionManager().SetSelectionRange(
        Range::Infinite(), 
        text_box::SelectionOption::SetCaretToEnd | text_box::SelectionOption::ScrollToCaret, 
        true);
}

}