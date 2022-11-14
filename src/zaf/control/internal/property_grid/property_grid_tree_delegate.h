#pragma once

#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/internal/property_grid/property_grid_split_distance_manager.h>

namespace zaf::internal {

class PropertyGridTreeDelegate : public TreeControlDelegate {
public:
    PropertyGridTreeDelegate();

    float EstimateItemHeight(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override {

        return 21;
    }

    std::shared_ptr<TreeItem> CreateItem(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

private:
    std::shared_ptr<PropertyGridSplitDistanceManager> split_distance_manager_;
};

}