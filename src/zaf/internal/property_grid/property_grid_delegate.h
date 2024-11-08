#pragma once

#include <zaf/control/internal/tree_control/tree_control_implementation.h>
#include <zaf/internal/property_grid/split_distance_manager.h>
#include <zaf/control/property_grid/type_config_factory.h>
#include <zaf/control/tree_control_delegate.h>

namespace zaf::internal {

class PropertyGridDelegate : public TreeControlDelegate, SubscriptionHost {
public:
    PropertyGridDelegate(
        const std::shared_ptr<property_grid::TypeConfigFactory>& type_config_factory,
        const std::shared_ptr<SplitDistanceManager>& split_distance_manager,
        const std::weak_ptr<zaf::internal::TreeControlImplementation>& tree_implementation);

    float EstimateItemHeight(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override {

        return 22;
    }

    std::shared_ptr<TreeItem> CreateItem(
        const std::shared_ptr<Object>& parent_item_data,
        std::size_t item_index,
        const std::shared_ptr<Object>& item_data) override;

private:
    void OnValueViewShouldSelect(const std::weak_ptr<Object>& weak_data);

private:
    std::shared_ptr<property_grid::TypeConfigFactory> type_config_factory_;
    std::shared_ptr<SplitDistanceManager> split_distance_manager_;
    std::weak_ptr<zaf::internal::TreeControlImplementation> tree_implementation_;
};

}