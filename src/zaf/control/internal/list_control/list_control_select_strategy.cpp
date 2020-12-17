#include <zaf/control/internal/list_control/list_control_select_strategy.h>
#include <zaf/base/define.h>
#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {
namespace internal {

bool ListControlSelectStrategy::ChangeIndexByKeyDown(
    const KeyMessage& key_message, 
    const std::optional<std::size_t>& previous_index, 
    std::size_t& new_index) {

    switch (key_message.wparam) {

        case VK_DOWN:
            if (!previous_index) {
                new_index = 0;
            }
            else if (*previous_index < GetItemHeightManager()->GetItemCount() - 1) {
                new_index = *previous_index + 1;
            }
            else {
                new_index = *previous_index;
            }
            return true;

        case VK_UP:
            if (!previous_index) {
                new_index = GetItemHeightManager()->GetItemCount() - 1;
            }
            else if (*previous_index > 0) {
                new_index = *previous_index - 1;
            }
            else {
                new_index = *previous_index;
            }
            return true;

        case VK_HOME:
            new_index = 0;
            return true;

        case VK_END:
            new_index = GetItemHeightManager()->GetItemCount() - 1;
            return true;

        default:
            return false;
    }
}

}
}