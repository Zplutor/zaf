#include <zaf/control/internal/list_control/list_control_simple_multiple_select_strategy.h>
#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <zaf/control/list_control.h>

namespace zaf {
namespace internal {

void ListControlSimpleMultipleSelectStrategy::BeginChangingSelectionByMouseDown(const Point& position, const MouseMessage& message) {

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


void ListControlSimpleMultipleSelectStrategy::ChangeSelectionByMouseMove(const Point& position, const MouseMessage& message) {

    //Cannot change selection while moving mouse.
}


void ListControlSimpleMultipleSelectStrategy::EndChangingSelectionByMouseUp(const Point& position, const MouseMessage& message) {

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


bool ListControlSimpleMultipleSelectStrategy::ChangeSelectionByKeyDown(const KeyMessage& message) {

    //Cannot change selection using keys.
    return false;
}

}
}