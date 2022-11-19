#pragma once

#include <zaf/control/tree_control_delegate.h>
#include <zaf/control/property_grid/internal/split_distance_manager.h>
#include <zaf/control/property_grid/type_config_factory.h>

namespace zaf::property_grid::internal {

class Delegate : public TreeControlDelegate {
public:
    Delegate(
        const std::shared_ptr<TypeConfigFactory>& type_config_factory,
        const std::shared_ptr<SplitDistanceManager>& split_distance_manager);

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
    std::shared_ptr<TypeConfigFactory> type_config_factory_;
    std::shared_ptr<SplitDistanceManager> split_distance_manager_;
};

}