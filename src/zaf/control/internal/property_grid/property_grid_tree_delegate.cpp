#include <zaf/control/internal/property_grid/property_grid_tree_delegate.h>
#include <zaf/base/as.h>
#include <zaf/control/internal/property_grid/property_grid_data.h>
#include <zaf/control/internal/property_grid/property_grid_item.h>
#include <zaf/creation.h>

namespace zaf::internal {

PropertyGridTreeDelegate::PropertyGridTreeDelegate() : 
    split_distance_manager_(std::make_shared<PropertyGridSplitDistanceManager>()) {

}


std::shared_ptr<TreeItem> PropertyGridTreeDelegate::CreateItem(
    const std::shared_ptr<Object>& parent_item_data,
    std::size_t item_index,
    const std::shared_ptr<Object>& item_data) {

    return Create<PropertyGridItem>(
        As<PropertyGridData>(item_data),
        split_distance_manager_);
}

}