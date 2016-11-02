#include <zaf/control/internal/list_control_single_select_strategy.h>
#include <zaf/control/internal/list_control_item_height_manager.h>
#include <zaf/control/list_control.h>

namespace zaf {
namespace internal {

void ListControlSingleSelectStrategy::BeginChangingSelectionByMouseDown(const Point& position, const MouseMessage& message) {
    SelectItemWithMouseEvent(position);
}


void ListControlSingleSelectStrategy::ChangeSelectionByMouseMove(const Point& position, const MouseMessage& message) {
    SelectItemWithMouseEvent(position);
}


void ListControlSingleSelectStrategy::EndChangingSelectionByMouseUp(const Point& position, const MouseMessage& message) {

    auto list_control = GetListControl();
    if (list_control != nullptr) {
        list_control->NotifySelectionChange();
    }
}


bool ListControlSingleSelectStrategy::ChangeSelectionByKeyDown(const KeyMessage& message) {

    auto list_control = GetListControl();
    if (list_control == nullptr) {
        return false;
    }
    
    std::size_t previous_selected_index = list_control->GetFirstSelectedItemIndex();
    std::size_t new_selected_index = InvalidIndex;

    bool change_index = ChangeIndexByKeyDown(message, previous_selected_index, new_selected_index);
    if (!change_index) {
        return false;
    }

    list_control->ReplaceSelection(new_selected_index, 1);
    list_control->ScrollToItemAtIndex(new_selected_index);
    list_control->NotifySelectionChange();
    return true;
}


void ListControlSingleSelectStrategy::SelectItemWithMouseEvent(const Point& position) {
    
    auto index_and_count = GetItemHeightManager()->GetItemIndexAndCount(position.y, position.y);
    if (index_and_count.second == 0) {
        return;
    }

    auto list_control = GetListControl();
    if (list_control != nullptr) {
        list_control->ReplaceSelection(index_and_count.first, index_and_count.second);
        list_control->ScrollToItemAtIndex(index_and_count.first);
    }
}

}
}