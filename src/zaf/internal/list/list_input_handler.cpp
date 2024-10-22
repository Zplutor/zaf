#include <zaf/internal/list/list_input_handler.h>
#include <zaf/base/as.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_control_part_context.h>
#include <zaf/rx/creation.h>

namespace zaf::internal {

ListInputHandler::~ListInputHandler() {

}


void ListInputHandler::HandleMouseDownEvent(const MouseDownInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto list_control = As<ListControl>(event_info.Sender());
    if (!list_control) {
        return;
    }

    list_control->SetIsFocused(true);

    auto position_in_container = 
        list_control->ItemContainer()->TranslateFromParent(event_info.PositionAtSender());

    auto item_index = Context().ItemHeightManager().GetItemIndex(position_in_container.y);
    if (item_index) {

        if (event_info.Message().MouseButton() == MouseButton::Left) {

            is_handling_mouse_event_ = true;
            list_control->CaptureMouse();

            auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
            selection_strategy.ChangeSelectionOnMouseDown(*item_index);
        }

        list_control->ScrollToItemAtIndex(*item_index);
    }

    event_info.MarkAsHandled();
}


void ListInputHandler::HandleMouseMoveEvent(const MouseMoveInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto list_control = As<ListControl>(event_info.Sender());
    if (!list_control) {
        return;
    }

    //Handle mouse move only when the list control is capturing the mouse.
    if (!list_control->IsCapturingMouse()) {
        return;
    }

    auto position_in_container =
        list_control->ItemContainer()->TranslateFromParent(event_info.PositionAtSender());

    auto item_index = Context().ItemHeightManager().GetItemIndex(position_in_container.y);
    if (item_index) {

        auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
        selection_strategy.ChangeSelectionOnMouseMove(*item_index);

        list_control->ScrollToItemAtIndex(*item_index);
    }

    event_info.MarkAsHandled();
}


void ListInputHandler::HandleMouseUpEvent(const MouseUpInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto list_control = As<ListControl>(event_info.Sender());
    if (!list_control) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        if (list_control->IsCapturingMouse()) {
            list_control->ReleaseMouse();
        }

        auto position_in_container =
            list_control->ItemContainer()->TranslateFromParent(event_info.PositionAtSender());

        auto item_index = Context().ItemHeightManager().GetItemIndex(position_in_container.y);
        if (item_index) {

            auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
            selection_strategy.ChangeSelectionOnMouseUp(*item_index);
        }

        is_handling_mouse_event_ = false;
        if (exit_handle_mouse_event_subject_) {
            auto observer = exit_handle_mouse_event_subject_->AsObserver();
            observer.OnNext(None{});
            observer.OnCompleted();
            exit_handle_mouse_event_subject_.reset();
        }

        event_info.MarkAsHandled();
    }
}


Observable<None> ListInputHandler::WhenNotHandlingMouseEvent() const {

    if (!is_handling_mouse_event_) {
        return zaf::rx::Just(None{});
    }

    if (!exit_handle_mouse_event_subject_) {
        exit_handle_mouse_event_subject_.emplace();
    }

    return exit_handle_mouse_event_subject_->AsObservable();
}


void ListInputHandler::HandleKeyDownEvent(const KeyDownInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto list_control = As<ListControl>(event_info.Sender());
    if (!list_control) {
        return;
    }

    auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
    auto new_selected_index = selection_strategy.ChangeSelectionOnKeyDown(event_info.Message());
    if (new_selected_index) {

        list_control->ScrollToItemAtIndex(*new_selected_index);
        event_info.MarkAsHandled();
    }
}

}