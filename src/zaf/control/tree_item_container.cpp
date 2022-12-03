#include <zaf/control/tree_item_container.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {

bool TreeItemContainer::OnKeyDown(const KeyMessage& message) {

    std::optional<bool> is_to_expand;
    if (message.VirtualKey() == VK_LEFT) {
        is_to_expand = false;
    }
    else if (message.VirtualKey() == VK_RIGHT) {
        is_to_expand = true;
    }
    
    if (is_to_expand.has_value()) {
        change_expand_state_event_.GetObserver().OnNext(*is_to_expand);
        return true;
    }

    return __super::OnKeyDown(message);
}

}