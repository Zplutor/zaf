#include <zaf/internal/list/list_input_handler.h>
#include <zaf/base/as.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_control_part_context.h>

namespace zaf::internal {

ListInputHandler::ListInputHandler(const ListControlPartContext* context) :
    ListControlPart(context) {

}


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

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        list_control->CaptureMouse();

        auto position_in_container = 
            list_control->ItemContainer()->TranslateFromParent(event_info.PositionAtSender());

        auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
        selection_strategy.BeginChangingSelectionByMouseDown(position_in_container);
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

    if (list_control->IsCapturingMouse()) {

        auto position_in_container =
            list_control->ItemContainer()->TranslateFromParent(event_info.PositionAtSender());

        auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
        selection_strategy.ChangeSelectionByMouseMove(position_in_container);

        event_info.MarkAsHandled();
    }
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

        auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
        selection_strategy.EndChangingSelectionByMouseUp(position_in_container);

        event_info.MarkAsHandled();
    }
}


void ListInputHandler::HandleKeyDownEvent(const KeyDownInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto& selection_strategy = Context().SelectionManager().SelectionStrategy();
    bool is_handled = selection_strategy.ChangeSelectionByKeyDown(event_info.Message());
    if (is_handled) {
        event_info.MarkAsHandled();
    }
}

}