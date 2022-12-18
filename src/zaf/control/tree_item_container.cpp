#include <zaf/control/tree_item_container.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {

void TreeItemContainer::OnKeyDown(const KeyDownInfo& event_info) {

    __super::OnKeyDown(event_info);

    if (event_info.IsHandled()) {
        return;
    }

    std::optional<bool> is_to_expand;
    if (event_info.Message().VirtualKey() == VK_LEFT) {
        is_to_expand = false;
    }
    else if (event_info.Message().VirtualKey() == VK_RIGHT) {
        is_to_expand = true;
    }
    
    if (is_to_expand.has_value()) {
        change_expand_state_event_.GetObserver().OnNext(*is_to_expand);
        event_info.MarkAsHandled();
    }
}

}