#include <zaf/control/internal/list_control_simple_multiple_select_strategy.h>
#include <zaf/control/list_control.h>
#include <zaf/control/internal/list_control_item_height_manager.h>

namespace zaf {
namespace internal {

void ListControlSimpleMultipleSelectStrategy::BeginChangingSelectionByMouseDown(const Point& position, const MouseMessage& message) {

    auto index_and_count = GetItemHeightManager()->GetItemIndexAndCount(position.y, position.y);
    if (index_and_count.second == 0) {
        return;
    }

    auto list_control = GetListControl();
    if (list_control != nullptr) {

        list_control->RevertSelection(index_and_count.first);
        list_control->ScrollToItemAtIndex(index_and_count.first);
    }
}


void ListControlSimpleMultipleSelectStrategy::ChangeSelectionByMouseMove(const Point& position, const MouseMessage& message) {

    //Cannot change selection while moving mouse.
}


void ListControlSimpleMultipleSelectStrategy::EndChangingSelectionByMouseUp(const Point& position, const MouseMessage& message) {

    auto list_control = GetListControl();
    if (list_control != nullptr) {
        list_control->NotifySelectionChange();
    }
}


bool ListControlSimpleMultipleSelectStrategy::ChangeSelectionByKeyDown(const KeyMessage& message) {

    //Cannot change selection using keys.
    return false;
}

}
}