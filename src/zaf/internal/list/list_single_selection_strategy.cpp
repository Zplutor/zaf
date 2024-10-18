#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/control/list_control.h>

namespace zaf {
namespace internal {

void ListSingleSelectionStrategy::BeginChangingSelectionByMouseDown(
    const Point& position,
    const MouseMessage& message) {

    SelectItemWithMouseEvent(position);
}


void ListSingleSelectionStrategy::ChangeSelectionByMouseMove(
    const Point& position,
    const MouseMessage& message) {

    SelectItemWithMouseEvent(position);
}


void ListSingleSelectionStrategy::EndChangingSelectionByMouseUp(
    const Point& position,
    const MouseMessage& message) {

    if (!mouse_selected_index_) {
        return;
    }

    GetListControl().NotifySelectionChange(
        ListSelectionChangeReason::ReplaceSelection, 
        *mouse_selected_index_,
        1);
}


bool ListSingleSelectionStrategy::ChangeSelectionByKeyDown(const KeyMessage& message) {

    auto& list_control = GetListControl();
    
    auto previous_selected_index = list_control.GetFirstSelectedItemIndex();
    std::size_t new_selected_index{};
    bool change_index = ChangeIndexByKeyDown(message, previous_selected_index, new_selected_index);
    if (!change_index) {
        return false;
    }

    list_control.ReplaceSelection(new_selected_index, 1);
    list_control.ScrollToItemAtIndex(new_selected_index);
    list_control.NotifySelectionChange(
        ListSelectionChangeReason::ReplaceSelection, 
        new_selected_index, 
        1);

    return true;
}


void ListSingleSelectionStrategy::SelectItemWithMouseEvent(const Point& position) {
    
    mouse_selected_index_ = GetItemHeightManager().GetItemIndex(position.y);
    if (!mouse_selected_index_) {
        return;
    }

    auto& list_control = GetListControl();
    list_control.ReplaceSelection(*mouse_selected_index_, 1);
    list_control.ScrollToItemAtIndex(*mouse_selected_index_);
}

}
}