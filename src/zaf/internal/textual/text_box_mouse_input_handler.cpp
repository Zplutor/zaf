#include <zaf/internal/textual/text_box_mouse_input_handler.h>
#include <zaf/base/as.h>
#include <zaf/base/auto_reset.h>
#include <zaf/base/log.h>
#include <zaf/control/text_box.h>
#include <zaf/control/textual/dynamic_inline_object.h>
#include <zaf/internal/textual/text_model.h>
#include <zaf/internal/textual/text_box_hit_test_manager.h>
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


void TextBoxMouseInputHandler::HandleMouseMove(const MouseMoveInfo& event_info) {

    auto& hit_test_manager = Context().HitTestManager();
    auto hit_test_result = hit_test_manager.HitTestAtPosition(event_info.PositionAtSource());

    HandleMouseOverInlineObject(hit_test_result, event_info.Message());

    if (!begin_selecting_index_) {
        return;
    }

    auto new_index = hit_test_manager.TextIndexFromHitTestResult(hit_test_result);
    SetCaretIndexByMouse(new_index, false);
}


void TextBoxMouseInputHandler::HandleMouseOverInlineObject(
    const HitTestPointResult& hit_test_result,
    const MouseMessage& mouse_message) {

    auto new_object = FindInlineObject(hit_test_result);
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
            window->Messager().PostWMSETCURSOR(mouse_message);
        }
    }
}


std::shared_ptr<textual::DynamicInlineObject> TextBoxMouseInputHandler::FindInlineObject(
    const HitTestPointResult& hit_test_result) const {

    if (hit_test_result.Metrics().IsText()) {
        return nullptr;
    }

    auto text_index = hit_test_result.Metrics().TextIndex();
    auto inline_object = Context().TextModel().StyledText().GetInlineObjectAtIndex(text_index);
    auto dynamic_inline_object = As<textual::DynamicInlineObject>(inline_object);
    if (!dynamic_inline_object) {
        return nullptr;
    }

    if (!dynamic_inline_object->HitTest(hit_test_result.IsInside())) {
        return nullptr;
    }

    return dynamic_inline_object;
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

    auto& hit_test_manager = Context().HitTestManager();
    auto hit_test_result = hit_test_manager.HitTestAtPosition(event_info.PositionAtSource());

    auto inline_object = FindInlineObject(hit_test_result);
    if (inline_object) {

        textual::MouseDownInfo event_info{ inline_object };
        inline_object->OnMouseDown(event_info);
        if (event_info.IsHandled()) {
            return;
        }
    }
    
    auto new_index = hit_test_manager.TextIndexFromHitTestResult(hit_test_result);
    SetCaretIndexByMouse(new_index, true);
}


void TextBoxMouseInputHandler::HandleMouseUp(const MouseUpInfo& event_info) {

    auto& text_box = Context().Owner();
    text_box.ReleaseMouse();

    begin_selecting_index_.reset();

    auto& hit_test_manager = Context().HitTestManager();
    auto hit_test_result = hit_test_manager.HitTestAtPosition(event_info.PositionAtSource());

    auto inline_object = FindInlineObject(hit_test_result);
    if (inline_object) {

        textual::MouseUpInfo event_info{ inline_object };
        inline_object->OnMouseUp(event_info);
    }
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