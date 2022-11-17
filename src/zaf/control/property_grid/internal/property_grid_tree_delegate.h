#pragma once

#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/property_grid/internal/property_grid_split_distance_manager.h>
#include <zaf/control/property_grid/type_config_factory.h>

namespace zaf::internal {

class PropertyGridTreeDelegate : public TreeControlDelegate {
public:
    PropertyGridTreeDelegate(
        const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory,
        const std::shared_ptr<PropertyGridSplitDistanceManager>& split_distance_manager);

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
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
    std::shared_ptr<PropertyGridSplitDistanceManager> split_distance_manager_;
};

}