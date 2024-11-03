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

    auto scroll_box = As<ScrollBox>(event_info.Sender());
    if (!scroll_box) {
        return;
    }

    if (!scroll_box->ViewportRect().Contains(event_info.PositionAtSender())) {
        return;
    }

    auto position_in_container = 
        scroll_box->TranslateToScrollContent(event_info.PositionAtSender());

    auto item_index = Parts().ItemHeightManager().GetItemIndex(position_in_container.y);

    bool is_handled{};
    if (event_info.Message().MouseButton() == MouseButton::Left) {
        is_handled = HandleLeftButtonDown(item_index);
    }
    else if (event_info.Message().MouseButton() == MouseButton::Right) {
        is_handled = HandleRightButtonDown(item_index, position_in_container);
    }

    if (is_handled) {
        event_info.MarkAsHandled();
    }
}


bool ListInputHandler::HandleLeftButtonDown(std::optional<std::size_t> item_index) {

    if (item_index) {

        Parts().FocusManager().ChangeFocusByUserInput(item_index);

        is_selecting_by_mouse_ = true;
        Parts().Owner().CaptureMouse();

        auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
        selection_strategy.ChangeSelectionOnMouseDown(*item_index);

        Parts().Core().ScrollToItemAtIndex(*item_index);
    }
    else {
        RestoreFocusToOwner();
    }

    return true;
}


bool ListInputHandler::HandleRightButtonDown(
    std::optional<std::size_t> item_index,
    const Point& position_in_container) {

    RestoreFocusToOwner();

    if (item_index) {
        PopupContextMenuOnItem(*item_index, position_in_container);
    }

    return true;
}


void ListInputHandler::RestoreFocusToOwner() {

    if (!Parts().Owner().ContainsFocus()) {
        Parts().FocusManager().ChangeFocusByUserInput(std::nullopt);
    }
}


void ListInputHandler::PopupContextMenuOnItem(
    std::size_t item_index,
    const Point& position_in_container) {

    if (!context_menu_callback_) {
        return;
    }

    auto visible_item = Parts().VisibleItemManager().GetVisibleItemAtIndex(item_index);
    if (!visible_item) {
        return;
    }

    auto data_source = Parts().Core().DataSource();
    if (!data_source) {
        return;
    }

    auto item_data = data_source->GetDataAtIndex(item_index);

    auto menu = context_menu_callback_(item_index, item_data);
    if (!menu) {
        return;
    }

    menu->PopupOnControl(
        visible_item,
        visible_item->TranslateFromParent(position_in_container));
}


void ListInputHandler::HandleMouseMoveEvent(const MouseMoveInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto scroll_box = As<ScrollBox>(event_info.Sender());
    if (!scroll_box) {
        return;
    }

    //Handle mouse move only when the list control is capturing the mouse.
    if (!scroll_box->IsCapturingMouse()) {
        return;
    }

    auto position_in_container =
        scroll_box->TranslateToScrollContent(event_info.PositionAtSender());

    auto item_index = Parts().ItemHeightManager().GetItemIndex(position_in_container.y);
    if (item_index) {

        Parts().FocusManager().ChangeFocusByUserInput(item_index);

        auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
        selection_strategy.ChangeSelectionOnMouseMove(*item_index);

        Parts().Core().ScrollToItemAtIndex(*item_index);
    }

    event_info.MarkAsHandled();
}


void ListInputHandler::HandleMouseUpEvent(const MouseUpInfo& event_info) {

    if (event_info.IsHandled()) {
        return;
    }

    auto scroll_box = As<ScrollBox>(event_info.Sender());
    if (!scroll_box) {
        return;
    }

    if (event_info.Message().MouseButton() == MouseButton::Left) {

        if (scroll_box->IsCapturingMouse()) {
            scroll_box->ReleaseMouse();
        }

        auto position_in_container = 
            scroll_box->TranslateToScrollContent(event_info.PositionAtSender());

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

    auto previous_index = Parts().FocusManager().FocusedIndex();
    if (!previous_index) {
        previous_index = Parts().SelectionStore().GetFirstSelectedIndex();
    }

    auto new_index = ChangeIndexByKey(event_info.Message().Key(), previous_index);
    if (!new_index) {
        return;
    }

    Parts().FocusManager().ChangeFocusByUserInput(new_index);

    auto& selection_strategy = Parts().SelectionManager().SelectionStrategy();
    selection_strategy.ChangeSelectionOnKeyDown(*new_index);

    Parts().Core().ScrollToItemAtIndex(*new_index);
    event_info.MarkAsHandled();
}


std::optional<std::size_t> ListInputHandler::ChangeIndexByKey(
    Key key,
    std::optional<std::size_t> previous_index) const {

    auto data_source = Parts().Core().DataSource();
    if (!data_source) {
        return std::nullopt;
    }

    auto data_count = data_source->GetDataCount();
    if (data_count == 0) {
        return std::nullopt;
    }

    switch (key) {
    case Key::Down:
        if (!previous_index) {
            return 0;
        }
        if (*previous_index < data_count - 1) {
            return *previous_index + 1;
        }
        return std::nullopt;

    case Key::Up:
        if (!previous_index) {
            return data_count - 1;
        }
        if (*previous_index > 0) {
            return *previous_index - 1;
        }
        return std::nullopt;

    case Key::Home:
        return 0;

    case Key::End:
        return data_count - 1;

    default:
        return std::nullopt;
    }
}

}