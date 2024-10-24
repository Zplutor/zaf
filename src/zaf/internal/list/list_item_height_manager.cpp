#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/base/error/check.h>
#include <zaf/internal/list/list_fixed_item_height_strategy.h>
#include <zaf/internal/list/list_variable_item_height_strategy.h>

namespace zaf::internal {

void ListItemHeightManager::ResetDataSource(std::weak_ptr<ListDataSource> data_source) {

    data_source_ = std::move(data_source);
    ReloadItemHeights();
}


void ListItemHeightManager::ResetDelegate(std::weak_ptr<ListControlDelegate> delegate) {

    delegate_ = std::move(delegate);
    ReloadItemHeights();
}


void ListItemHeightManager::ReloadItemHeights() {

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (delegate->HasVariableItemHeight()) {
        strategy_ = std::make_unique<ListVariableItemHeightStrategy>(data_source_, delegate_);
    }
    else {
        strategy_ = std::make_unique<ListFixedItemHeightStrategy>(data_source_, delegate_);
    }

    strategy_->Initialize();
}


std::pair<float, float> ListItemHeightManager::GetItemPositionAndHeight(std::size_t index) const {
    if (strategy_) {
        return strategy_->GetItemPositionAndHeight(index);
    }
    return {};
}


std::optional<std::size_t> ListItemHeightManager::GetItemIndex(float position) const {

    if (position < 0) {
        return std::nullopt;
    }

    if (!strategy_) {
        return std::nullopt;
    }

    return strategy_->GetItemIndex(position);
}


std::pair<std::size_t, std::size_t> ListItemHeightManager::GetItemRange(
    float begin_position, 
    float end_position) const {

    if (begin_position < 0 || begin_position >= end_position) {
        return {};
    }

    if (!strategy_) {
        return {};
    }

    return strategy_->GetItemRange(begin_position, end_position);
}


float ListItemHeightManager::GetTotalHeight() const {
    if (strategy_) {
        return strategy_->GetTotalHeight();
    }
    return 0;
}


void ListItemHeightManager::OnDataAdded(const ListDataAddedInfo& event_info) {
    if (strategy_) {
        strategy_->OnDataAdded(event_info);
    }
}


void ListItemHeightManager::OnDataRemoved(const ListDataRemovedInfo& event_info) {
    if (strategy_) {
        strategy_->OnDataRemoved(event_info);
    }
}


void ListItemHeightManager::OnDataUpdated(const ListDataUpdatedInfo& event_info) {
    if (strategy_) {
        strategy_->OnDataUpdated(event_info);
    }
}


void ListItemHeightManager::OnDataMoved(const ListDataMovedInfo& event_info) {
    if (strategy_) {
        strategy_->OnDataMoved(event_info);
    }
}

}