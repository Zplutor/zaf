#include <zaf/control/internal/list_control/list_control_select_strategy.h>
#include <zaf/base/define.h>
#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <zaf/window/message/keyboard_message.h>

namespace zaf {
namespace internal {

bool ListControlSelectStrategy::ChangeIndexByKeyDown(const KeyMessage& key_message, std::size_t previous_index, std::size_t& new_index) {

    new_index = previous_index;

    switch (key_message.wparam) {

        case VK_DOWN:
            if (new_index == InvalidIndex) {
                new_index = 0;
            }
            else if (new_index < GetItemHeightManager()->GetItemCount() - 1) {
                ++new_index;
            }
            return true;

        case VK_UP:
            if (new_index == InvalidIndex) {
                new_index = GetItemHeightManager()->GetItemCount() - 1;
            }
            else if (new_index > 0) {
                --new_index;
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