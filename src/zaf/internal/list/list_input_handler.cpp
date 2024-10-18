#include <zaf/internal/list/list_input_handler.h>
#include <zaf/base/as.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_extended_multiple_selection_strategy.h>
#include <zaf/internal/list/list_selection_strategy.h>
#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_simple_multiple_selection_strategy.h>

namespace zaf::internal {

ListInputHandler::ListInputHandler(const ListControlPartContext* context) :
    ListControlPart(context) {

}


ListInputHandler::~ListInputHandler() {

}


void ListInputHandler::ResetSelectionStrategy(SelectionMode selection_mode) {

    selection_strategy_ = [this, selection_mode]() -> std::unique_ptr<ListSelectionStrategy> {
    
        auto context = &Context();

        switch (selection_mode) {
        case SelectionMode::Single:
            return std::make_unique<ListSingleSelectionStrategy>(context);

        case SelectionMode::SimpleMultiple:
            return std::make_unique<ListSimpleMultipleSelectionStrategy>(context);

        case SelectionMode::ExtendedMultiple:
            return std::make_unique<ListExtendedMultipleSelectionStrategy>(context);

        default:
            return std::make_unique<ListNoSelectionStrategy>(context);
        }
    }();
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

        selection_strategy_->BeginChangingSelectionByMouseDown(
            position_in_container, 
            event_info.Message());
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

        selection_strategy_->ChangeSelectionByMouseMove(
            position_in_container,
            event_info.Message());

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

        selection_strategy_->EndChangingSelectionByMouseUp(
            position_in_container,
            event_info.Message());

        event_info.MarkAsHandled();
    }
}


void ListInputHandler::HandleKeyDownEvent(const KeyDownInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    bool is_handled = selection_strategy_->ChangeSelectionByKeyDown(event_info.Message());
    if (is_handled) {
        event_info.MarkAsHandled();
    }
}

}