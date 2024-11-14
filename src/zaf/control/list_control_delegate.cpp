#include <zaf/control/list_control_delegate.h>
#include <zaf/control/label_list_item.h>

namespace zaf {

const std::shared_ptr<ListControlDelegate>& ListControlDelegate::Default() {
    static auto default_instance = std::make_shared<ListControlDelegate>();
    return default_instance;
}


std::shared_ptr<ListItem> ListControlDelegate::CreateItem(
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    return Create<LabelListItem>();
}

}