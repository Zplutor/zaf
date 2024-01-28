#include <zaf/control/text_box/internal/text_box_mouse_input_handler.h>
#include <zaf/control/text_box.h>
#include <zaf/control/text_box/internal/text_box_module_context.h>
#include <zaf/control/text_box/internal/text_box_selection_manager.h>

namespace zaf::internal {

TextBoxMouseInputHandler::TextBoxMouseInputHandler(TextBoxModuleContext* context) : TextBoxModule(context) {

}


void TextBoxMouseInputHandler::HandleMouseDown(const MouseDownInfo& event_info) {

    auto& text_box = Context().Owner();
    text_box.SetIsFocused(true);
    text_box.CaptureMouse();

    auto new_index = text_box.FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, true);
}


void TextBoxMouseInputHandler::HandleMouseMove(const MouseMoveInfo& event_info) {

    if (!begin_mouse_select_index_) {
        return;
    }

    auto new_index = Context().Owner().FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, false);
}


void TextBoxMouseInputHandler::HandleMouseUp(const MouseUpInfo& event_info) {

    if (!begin_mouse_select_index_) {
        return;
    }

    auto& text_box = Context().Owner();
    text_box.ReleaseMouse();

    auto new_index = text_box.FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, false);

    begin_mouse_select_index_.reset();
}


void TextBoxMouseInputHandler::SetCaretIndexByMouse(std::size_t caret_index, bool begin_selection) {

    if (begin_selection) {
        begin_mouse_select_index_ = caret_index;
    }
    else {
        if (!begin_mouse_select_index_) {
            return;
        }
    }

    Range selection_range;
    text_box::SelectionOption selection_option{ text_box::SelectionOption::ScrollToCaret };

    if (*begin_mouse_select_index_ < caret_index) {
        selection_range = Range::FromIndexPair(*begin_mouse_select_index_, caret_index);
        selection_option |= text_box::SelectionOption::SetCaretToEnd;
    }
    else {
        selection_range = Range::FromIndexPair(caret_index , *begin_mouse_select_index_);
        selection_option |= text_box::SelectionOption::SetCaretToBegin;
    }

    Context().SelectionManager().SetSelectionRange(selection_range, selection_option, true);
}

}