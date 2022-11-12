#include <zaf/control/internal/list_control/list_control_item_height_manager.h>
#include <zaf/control/internal/list_control/list_control_fixed_item_height_strategy.h>
#include <zaf/control/internal/list_control/list_control_variable_item_height_strategy.h>

namespace zaf::internal {

ListControlItemHeightManager::ListControlItemHeightManager(
    const std::shared_ptr<ListDataSource>& item_source) : data_source_(item_source) {

    RegisterDataSourceEvents();
}


ListControlItemHeightManager::~ListControlItemHeightManager() {
    UnregisterDataSourceEvents();
}


void ListControlItemHeightManager::RegisterDataSourceEvents() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    data_source_subscriptions_ += data_source->DataAddEvent().Subscribe(
        std::bind(
            &ListControlItemHeightManager::ItemAdd, 
            this, 
            std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataRemoveEvent().Subscribe(
        std::bind(
            &ListControlItemHeightManager::ItemRemove, 
            this,
            std::placeholders::_1));

    data_source_subscriptions_ += data_source->DataUpdateEvent().Subscribe(
        std::bind(
            &ListControlItemHeightManager::ItemUpdate, 
            this, 
            std::placeholders::_1));
}


void ListControlItemHeightManager::UnregisterDataSourceEvents() {

    data_source_subscriptions_.Clear();
}


void ListControlItemHeightManager::ResetDelegate(
    const std::shared_ptr<ListControlDelegate>& delegate) {

    delegate_ = delegate;
    ReloadItemHeights();
}


void ListControlItemHeightManager::ReloadItemHeights() {

    auto data_source = data_source_.lock();
    if (!data_source) {
        return;
    }

    auto delegate = delegate_.lock();
    if (!delegate) {
        return;
    }

    if (delegate->HasVariableItemHeight()) {
        strategy_ = std::make_unique<ListControlVariableItemHeightStrategy>();
    }
    else {
        strategy_ = std::make_unique<ListControlFixedItemHeightStrategy>();
    }

    strategy_->Initialize(*data_source, *delegate);
}


std::pair<float, float> ListControlItemHeightManager::GetItemPositionAndHeight(
    std::size_t index) const {

    if (strategy_) {
        if (index < strategy_->ItemCount()) {
            return strategy_->GetItemPositionAndHeight(index);
        }
    }
    return {};
}


std::pair<std::size_t, std::size_t> ListControlItemHeightManager::GetItemIndexAndCount(
    float begin_position, 
    float end_position) const {

    //It is meaningless if the end_position is less than begin_position.
    if (begin_position <= end_position) {
        if (strategy_) {
            return strategy_->GetItemIndexAndCount(begin_position, end_position);
        }
    }
    return {};
}


float ListControlItemHeightManager::GetTotalHeight() const {

    if (strategy_) {
        return strategy_->GetTotalHeight();
    }
    return 0;
}


void ListControlItemHeightManager::ItemAdd(const ListDataSourceDataAddInfo& event_info) {

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

    if (event_info.index > strategy_->ItemCount()) {
        ZAF_ALERT();
        return;
    }

    strategy_->OnItemAdd(event_info, *data_source, *delegate);
}


void ListControlItemHeightManager::ItemRemove(const ListDataSourceDataRemoveInfo& event_info) {

    if (!strategy_) {
        return;
    }

    if (event_info.index >= strategy_->ItemCount()) {
        ZAF_ALERT();
        return;
    }

    if (event_info.count > strategy_->ItemCount() - event_info.index) {
        ZAF_ALERT();
        return;
    }

    strategy_->OnItemRemove(event_info);
}


void ListControlItemHeightManager::ItemUpdate(const ListDataSourceDataUpdateInfo& event_info) {

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

    if (event_info.index >= strategy_->ItemCount()) {
        ZAF_ALERT();
        return;
    }

    if (event_info.count > strategy_->ItemCount() - event_info.index) {
        ZAF_ALERT();
        return;
    }

    strategy_->OnItemUpdate(event_info, *data_source, *delegate);
}

}