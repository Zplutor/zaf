#pragma once

#include <zaf/control/event/focus_event_info.h>
#include <zaf/internal/tree/tree_core.h>
#include <zaf/control/property_grid_delegate.h>
#include <zaf/control/tree_control_delegate.h>
#include <zaf/internal/property_grid/split_distance_manager.h>

namespace zaf::internal {

class PropertyGridItemManager : public TreeControlDelegate, rx::DisposableHost {
public:
    PropertyGridItemManager(
        const std::shared_ptr<PropertyGridDelegate>& delegate,
        const std::shared_ptr<SplitDistanceManager>& split_distance_manager,
        const std::weak_ptr<zaf::internal::TreeCore>& tree_core);

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
    void OnValueEditorGainedFocus(
        const std::weak_ptr<Object>& data,
        const FocusGainedInfo& event_info);

private:
    std::shared_ptr<PropertyGridDelegate> delegate_;
    std::shared_ptr<SplitDistanceManager> split_distance_manager_;
    std::weak_ptr<zaf::internal::TreeCore> tree_core_;
};

}