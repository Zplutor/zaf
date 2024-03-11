#include <zaf/internal/textual/text_box_mouse_input_handler.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/control/text_box.h>
#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/internal/textual/text_box_module_context.h>
#include <zaf/internal/textual/text_box_selection_manager.h>
#include <zaf/window/window.h>

namespace zaf::internal {

TextBoxMouseInputHandler::TextBoxMouseInputHandler(TextBoxModuleContext* context) : 
    TextBoxModule(context) {

}


void TextBoxMouseInputHandler::Initialize() {

    Subscriptions() += Context().SelectionManager().SelectionChangedEvent().Subscribe(
        std::bind(&TextBoxMouseInputHandler::OnSelectionChanged, this));
}


void TextBoxMouseInputHandler::HandleMouseCursorChanging(
    const MouseCursorChangingInfo& event_info) {

    auto object = mouse_over_object_.lock();
    if (!object) {
        return;
    }

    textual::MouseCursorChangingInfo object_event_info{ object };
    object->OnMouseCursorChanging(object_event_info);

    if (object_event_info.IsHandled()) {
        event_info.MarkAsHandled();
    }
}


void TextBoxMouseInputHandler::HandleMouseDown(const MouseDownInfo& event_info) {

    auto& text_box = Context().Owner();
    text_box.SetIsFocused(true);
    text_box.CaptureMouse();

    auto new_index = text_box.FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, true);
}


void TextBoxMouseInputHandler::HandleMouseMove(const MouseMoveInfo& event_info) {

    HandleMouseOverInlineObject(event_info);

    if (!begin_selecting_index_) {
        return;
    }

    auto new_index = Context().Owner().FindIndexAtPosition(event_info.PositionAtSource());
    SetCaretIndexByMouse(new_index, false);
}


void TextBoxMouseInputHandler::HandleMouseOverInlineObject(const MouseMoveInfo& event_info) {

    auto new_object = FindMouseOverInlineObject(event_info.PositionAtSource());
    auto old_object = mouse_over_object_.lock();
    if (old_object == new_object) {
        return;
    }

    mouse_over_object_ = new_object;

    if (old_object) {
        old_object->OnMouseLeave(textual::MouseLeaveInfo{ old_object });
    }

    if (new_object) {
        new_object->OnMouseEnter(textual::MouseEnterInfo{ new_object });

        //See also Window::SetMouseOverControl()
        auto window = Context().Owner().Window();
        if (window) {
            window->Messager().PostWMSETCURSOR(event_info.Message());
        }
    }
}


std::shared_ptr<textual::DynamicInlineObject> TextBoxMouseInputHandler::FindMouseOverInlineObject(
    const Point& position_in_owner) const {

    const auto& text_box = Context().Owner();

    auto hit_test_result = text_box.HitTestAtPosition(position_in_owner);
    if (hit_test_result.Metrics().IsText()) {
        return nullptr;
    }

    auto text_index = hit_test_result.Metrics().TextIndex();
    auto inline_object = Context().TextModel().StyledText().GetInlineObjectAtIndex(text_index);
    auto dynamic_inline_object = As<textual::DynamicInlineObject>(inline_object);
    if (!dynamic_inline_object) {
        return nullptr;
    }

    textual::HitTestInfo object_hit_test_info{ hit_test_result.IsInside() };
    auto object_hit_test_result = dynamic_inline_object->HitTest(object_hit_test_info);
    if (object_hit_test_result == textual::HitTestResult::None) {
        return nullptr;
    }

    return dynamic_inline_object;
}


void TextBoxMouseInputHandler::HandleMouseUp(const MouseUpInfo& event_info) {

    if (!begin_selecting_index_) {
        return;
    }

    auto& text_box = Context().Owner();
    text_box.ReleaseMouse();

    begin_selecting_index_.reset();
}


void TextBoxMouseInputHandler::SetCaretIndexByMouse(
    std::size_t caret_index, 
    bool begin_selection) {

    if (begin_selection) {
        begin_selecting_index_ = caret_index;
    }
    else {
        if (!begin_selecting_index_) {
            return;
        }
    }

    Range selection_range;
    textual::SelectionOption selection_option{ textual::SelectionOption::ScrollToCaret };

    if (*begin_selecting_index_ < caret_index) {
        selection_range = Range::FromIndexPair(*begin_selecting_index_, caret_index);
        selection_option |= textual::SelectionOption::SetCaretToEnd;
    }
    else {
        selection_range = Range::FromIndexPair(caret_index , *begin_selecting_index_);
        selection_option |= textual::SelectionOption::SetCaretToBegin;
    }

    auto auto_reset = MakeAutoReset(is_setting_selection_range_, true);
    Context().SelectionManager().SetSelectionRange(selection_range, selection_option, true);
}


void TextBoxMouseInputHandler::OnSelectionChanged() {

    if (is_setting_selection_range_) {
        return;
    }

    if (!begin_selecting_index_) {
        return;
    }

    //Reset begin selecting index to the caret index if it is changed during mouse dragging.
    *begin_selecting_index_ = Context().SelectionManager().CaretIndex();
}

}