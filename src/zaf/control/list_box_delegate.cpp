#include <zaf/control/list_box_delegate.h>

namespace zaf {

float ListBoxDelegate::EstimateItemHeight(
    std::size_t item_index, 
    const std::shared_ptr<dynamic::Object>& item_data) {

    return item_height_;
}

}