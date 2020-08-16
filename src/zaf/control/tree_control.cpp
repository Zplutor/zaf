#include <zaf/control/tree_control.h>

namespace zaf {

void TreeControl::ItemSourceChange(const std::shared_ptr<ListItemSource>& previous_item_source) {

    __super::ItemSourceChange(previous_item_source);

    item_source_ = std::dynamic_pointer_cast<TreeItemSource>(GetItemSource());
}


std::size_t TreeItemSource::GetItemCount() {
    return {};
}


std::shared_ptr<ListItem> TreeItemSource::CreateItem(std::size_t index) {
    return {};
}

}