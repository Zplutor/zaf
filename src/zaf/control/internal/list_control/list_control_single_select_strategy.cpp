#include <zaf/control/internal/list_control/list_control_single_select_strategy.h>
#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <zaf/control/list_control.h>

namespace zaf {
namespace internal {

void ListControlSingleSelectStrategy::BeginChangingSelectionByMouseDown(
    const Point& position,
    const MouseMessage& message) {

    SelectItemWithMouseEvent(position);
}


void ListControlSingleSelectStrategy::ChangeSelectionByMouseMove(
    const Point& position,
    const MouseMessage& message) {

    SelectItemWithMouseEvent(position);
}


void ListControlSingleSelectStrategy::EndChangingSelectionByMouseUp(
    const Point& position,
    const MouseMessage& message) {

    if (!mouse_selected_index_) {
        return;
    }

    auto list_control = GetListControl();
    if (list_control) {

        list_control->NotifySelectionChange(
            ListSelectionChangeReason::ReplaceSelection, 
            *mouse_selected_index_,
            1);
    }
}


bool ListControlSingleSelectStrategy::ChangeSelectionByKeyDown(const KeyMessage& message) {

    auto list_control = GetListControl();
    if (!list_control) {
        return false;
    }
    
    auto previous_selected_index = list_control->GetFirstSelectedItemIndex();
    std::size_t new_selected_index{};
    bool change_index = ChangeIndexByKeyDown(message, previous_selected_index, new_selected_index);
    if (!change_index) {
        return false;
    }

    list_control->ReplaceSelection(new_selected_index, 1);
    list_control->ScrollToItemAtIndex(new_selected_index);
    list_control->NotifySelectionChange(
        ListSelectionChangeReason::ReplaceSelection, 
        new_selected_index, 
        1);

    return true;
}


void ListControlSingleSelectStrategy::SelectItemWithMouseEvent(const Point& position) {
    
    mouse_selected_index_ = GetItemHeightManager()->GetItemIndex(position.y);
    if (!mouse_selected_index_) {
        return;
    }

    auto list_control = GetListControl();
    if (list_control) {

        list_control->ReplaceSelection(*mouse_selected_index_, 1);
        list_control->ScrollToItemAtIndex(*mouse_selected_index_);
    }
}

}
}