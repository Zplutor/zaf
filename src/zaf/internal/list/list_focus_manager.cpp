#include <zaf/internal/list/list_focus_manager.h>
#include <zaf/base/auto_reset.h>
#include <zaf/internal/list/list_control_parts_context.h>

namespace zaf::internal {

std::optional<std::size_t> ListFocusManager::FocusedIndex() const noexcept {
    return store_.Index();
}


void ListFocusManager::ChangeFocusByUserInput(std::optional<std::size_t> focused_index) {
    SetFocusToProperControl(focused_index);
}


void ListFocusManager::ChangeFocusByOwnerFocusGainedEvent(const FocusGainedInfo& event_info) {

    std::shared_ptr<ListItem> list_item;
    for (auto control = As<Control>(event_info.Source()); control; control = control->Parent()) {

        list_item = As<ListItem>(control);
        if (list_item) {
            break;
        }

        if (control.get() == &Parts().Owner()) {
            break;
        }
    }

    //Focus is set to one of the list item, record its index.
    //If the index of the list item is not found, which is an abnormal case, we leave the recorded 
    //index untouched.
    if (list_item) {
        auto item_index = Parts().VisibleItemManager().GetIndexOfVisibleItem(*list_item);
        if (item_index) {
            store_.SetIndex(item_index);
        }
    }
    //Focus is set to whatever that is not a list item, and current is not removing visible items,
    //clear the recorded index.
    else {
        if (!is_removing_visible_item_) {
            store_.SetIndex(std::nullopt);
        }
    }
}


void ListFocusManager::ChangeFocusByOwnerFocusLostEvent(const FocusLostInfo& event_info) {

}


void ListFocusManager::ChangeFocusByAddingData(const Range& added_range) {

    store_.AddRange(added_range);
    SetFocusToProperControlIfCan();
}


void ListFocusManager::ChangeFocusByRemovingData(const Range& removed_range) {

    store_.RemoveRange(removed_range);
    SetFocusToProperControlIfCan();
}


void ListFocusManager::ChangeFocusByMovingData(std::size_t from, std::size_t to) {

    store_.MoveIndex(from, to);
    SetFocusToProperControlIfCan();
}


void ListFocusManager::ChangeFocusBeforeRemovingVisibleItem(const ListItem& item) {

    if (item.IsFocused()) {
        auto auto_reset = MakeAutoReset(is_removing_visible_item_, true);
        Parts().Owner().SetIsFocused(true);
    }
}


void ListFocusManager::ChangeFocusAfterAddingVisibleItem(ListItem& item, std::size_t index) {

    if (!Parts().Owner().ContainsFocus()) {
        return;
    }

    if (FocusedIndex() == index) {
        item.SetIsFocused(true);
    }
}


void ListFocusManager::ChangeFocusByReloading() {

    store_.SetIndex(std::nullopt);
    SetFocusToProperControlIfCan();
}


void ListFocusManager::SetFocusToProperControlIfCan() const {

    if (Parts().Owner().ContainsFocus()) {
        SetFocusToProperControl(store_.Index());
    }
}


void ListFocusManager::SetFocusToProperControl(std::optional<std::size_t> focused_index) const {

    //This method is not allowed to be re-entered.
    ZAF_EXPECT(!is_setting_focus_to_control_);
    auto auto_reset = MakeAutoReset(is_setting_focus_to_control_, true);

    if (!focused_index) {
        Parts().Owner().SetIsFocused(true);
        return;
    }

    auto& visible_item_manager = Parts().VisibleItemManager();
    auto visible_item = visible_item_manager.GetVisibleItemAtIndex(*focused_index);
    if (visible_item) {
        visible_item->SetIsFocused(true);
    }
    else {
        Parts().Owner().SetIsFocused(true);
    }
}

}