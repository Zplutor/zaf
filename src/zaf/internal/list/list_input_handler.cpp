#include <zaf/internal/list/list_input_handler.h>
#include <zaf/base/as.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/rx/creation.h>

namespace zaf::internal {

void ListInputHandler::HandleMouseDownEvent(const MouseDownInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto list_control = As<ListControl>(event_info.Sender());
    if (!list_control) {
        return;
    }

    if (!list_control->ViewportRect().Contain(event_info.PositionAtSender())) {
        return;
    }

    auto position_in_container = 
        list_control->TranslateToScrollContent(event_info.PositionAtSender());

    auto item_index = Parts().ItemHeightManager().GetItemIndex(position_in_container.y);

    bool is_handled{};
    if (event_info.Message().MouseButton() == MouseButton::Left) {
        is_handled = HandleLeftButtonDown(*list_control, item_index);
    }
    else if (event_info.Message().MouseButton() == MouseButton::Right) {
        is_handled = HandleRightButtonDown(*list_control, item_index, position_in_container);
    }

    if (is_handled) {
        event_info.MarkAsHandled();
    }
}


bool ListInputHandler::HandleLeftButtonDown(
    ListControl& list_control, 
    std::optional<std::size_t> item_index) {

    if (item_index) {

        Parts().FocusStore().SetFocusedIndex(item_index);

        is_selecting_by_mouse_ = true;
        list_control.CaptureMouse();

        auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
        selection_strategy.ChangeSelectionOnMouseDown(*item_index);

        list_control.ScrollToItemAtIndex(*item_index);
    }
    else {
        RestoreFocusToListControl(list_control);
    }

    return true;
}


bool ListInputHandler::HandleRightButtonDown(
    ListControl& list_control, 
    std::optional<std::size_t> item_index,
    const Point& position_in_container) {

    RestoreFocusToListControl(list_control);

    if (item_index) {
        PopupContextMenuOnItem(list_control, *item_index, position_in_container);
    }

    return true;
}


void ListInputHandler::RestoreFocusToListControl(ListControl& list_control) {

    if (!list_control.ContainsFocus()) {
        auto& focus_store = Parts().FocusStore();
        focus_store.SetFocusedIndex(focus_store.FocusedIndex());
    }
}


void ListInputHandler::PopupContextMenuOnItem(
    ListControl& list_control, 
    std::size_t item_index,
    const Point& position_in_container) {

    auto visible_item = Parts().VisibleItemManager().GetVisibleItemAtIndex(item_index);
    if (!visible_item) {
        return;
    }

    auto data_source = Parts().Core().DataSource();
    if (!data_source) {
        return;
    }

    auto item_data = data_source->GetDataAtIndex(item_index);

    ListControlContextMenuInfo event_info{ 
        As<ListControl>(list_control.shared_from_this()),
        item_index,
        item_data,
    };

    context_menu_event_.AsObserver().OnNext(event_info);
    if (!event_info.Menu()) {
        return;
    }

    event_info.Menu()->PopupOnControl(
        visible_item,
        visible_item->TranslateFromParent(position_in_container));
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
        list_control->TranslateToScrollContent(event_info.PositionAtSender());

    auto item_index = Parts().ItemHeightManager().GetItemIndex(position_in_container.y);
    if (item_index) {

        Parts().FocusStore().SetFocusedIndex(item_index);

        auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
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

    if (!list_control->ViewportRect().Contain(event_info.PositionAtSender())) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        if (list_control->IsCapturingMouse()) {
            list_control->ReleaseMouse();
        }

        auto position_in_container = 
            list_control->TranslateToScrollContent(event_info.PositionAtSender());

        auto item_index = Parts().ItemHeightManager().GetItemIndex(position_in_container.y);
        if (item_index) {

            auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
            selection_strategy.ChangeSelectionOnMouseUp(*item_index);
        }

        is_selecting_by_mouse_ = false;
        if (exit_select_by_mouse_subject_) {
            auto observer = exit_select_by_mouse_subject_->AsObserver();
            observer.OnNext(None{});
            observer.OnCompleted();
            exit_select_by_mouse_subject_.reset();
        }

        event_info.MarkAsHandled();
    }
}


Observable<None> ListInputHandler::WhenNotSelectingByMouse() const {

    if (!is_selecting_by_mouse_) {
        return zaf::rx::Just(None{});
    }

    if (!exit_select_by_mouse_subject_) {
        exit_select_by_mouse_subject_.emplace();
    }

    return exit_select_by_mouse_subject_->AsObservable();
}


void ListInputHandler::HandleKeyDownEvent(const KeyDownInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto list_control = As<ListControl>(event_info.Sender());
    if (!list_control) {
        return;
    }

    auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
    auto new_selected_index = selection_strategy.ChangeSelectionOnKeyDown(event_info.Message());
    if (new_selected_index) {

        list_control->ScrollToItemAtIndex(*new_selected_index);
        event_info.MarkAsHandled();
    }
}

}