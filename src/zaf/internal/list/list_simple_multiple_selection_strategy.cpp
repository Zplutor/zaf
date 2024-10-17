#include <zaf/internal/list/list_simple_multiple_selection_strategy.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/control/list_control.h>

namespace zaf {
namespace internal {

void ListSimpleMultipleSelectionStrategy::BeginChangingSelectionByMouseDown(const Point& position, const MouseMessage& message) {

    mouse_selected_index_ = GetItemHeightManager()->GetItemIndex(position.y);
    if (!mouse_selected_index_) {
        return;
    }

    auto list_control = GetListControl();
    if (list_control) {

        is_mouse_selected_index_selected_ = 
            list_control->RevertSelection(*mouse_selected_index_);

        list_control->ScrollToItemAtIndex(*mouse_selected_index_);
    }
}


void ListSimpleMultipleSelectionStrategy::ChangeSelectionByMouseMove(const Point& position, const MouseMessage& message) {

    //Cannot change selection while moving mouse.
}


void ListSimpleMultipleSelectionStrategy::EndChangingSelectionByMouseUp(const Point& position, const MouseMessage& message) {

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


bool ListSimpleMultipleSelectionStrategy::ChangeSelectionByKeyDown(const KeyMessage& message) {

    //Cannot change selection using keys.
    return false;
}

}
}