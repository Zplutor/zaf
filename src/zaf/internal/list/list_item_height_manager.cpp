#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/base/error/check.h>
#include <zaf/internal/list/list_fixed_item_height_strategy.h>
#include <zaf/internal/list/list_variable_item_height_strategy.h>

namespace zaf::internal {

void ListItemHeightManager::RegisterDataSourceEvents() {

    data_source_subs_.Clear();

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subs_ += data_source->DataAddedEvent().Subscribe(
        std::bind_front(&ListItemHeightManager::OnDataAdded, this));

    data_source_subs_ += data_source->DataRemovedEvent().Subscribe(
        std::bind_front(&ListItemHeightManager::OnDataRemoved, this));

    data_source_subs_ += data_source->DataUpdatedEvent().Subscribe(
        std::bind_front(&ListItemHeightManager::OnDataUpdated, this));

    data_source_subs_ += data_source->DataMovedEvent().Subscribe(
        std::bind_front(&ListItemHeightManager::OnDataMoved, this));
}


void ListItemHeightManager::ResetDataSource(std::weak_ptr<ListDataSource> data_source) {

    data_source_ = std::move(data_source);
    RegisterDataSourceEvents();
    ReloadItemHeights();
}


void ListItemHeightManager::ResetDelegate(
    const std::weak_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    ReloadItemHeights();
}


void ListItemHeightManager::ReloadItemHeights() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (delegate->HasVariableItemHeight()) {
        strategy_ = std::make_unique<ListVariableItemHeightStrategy>();
    }
    else {
        strategy_ = std::make_unique<ListFixedItemHeightStrategy>();
    }

    strategy_->Initialize(*data_source, *delegate);
}


std::pair<float, float> ListItemHeightManager::GetItemPositionAndHeight(
    std::size_t index) const {

    if (strategy_) {
        if (index < strategy_->ItemCount()) {
            return strategy_->GetItemPositionAndHeight(index);
        }
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

    if (!strategy_) {
        return;
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (event_info.Index() > strategy_->ItemCount()) {
        ZAF_ALERT();
        return;
    }

    strategy_->OnDataAdded(event_info, *data_source, *delegate);
}


void ListItemHeightManager::OnDataRemoved(const ListDataRemovedInfo& event_info) {

    if (!strategy_) {
        return;
    }

    if (event_info.Index() >= strategy_->ItemCount()) {
        ZAF_ALERT();
        return;
    }

    if (event_info.Count() > strategy_->ItemCount() - event_info.Index()) {
        ZAF_ALERT();
        return;
    }

    strategy_->OnDataRemoved(event_info);
}


void ListItemHeightManager::OnDataUpdated(const ListDataUpdatedInfo& event_info) {

    if (!strategy_) {
        return;
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (event_info.Index() >= strategy_->ItemCount()) {
        ZAF_ALERT();
        return;
    }

    if (event_info.Count() > strategy_->ItemCount() - event_info.Index()) {
        ZAF_ALERT();
        return;
    }

    strategy_->OnDataUpdated(event_info, *data_source, *delegate);
}


void ListItemHeightManager::OnDataMoved(const ListDataMovedInfo& event_info) {

    if (!strategy_) {
        return;
    }

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    strategy_->OnDataMoved(event_info, *data_source, *delegate);
}

}