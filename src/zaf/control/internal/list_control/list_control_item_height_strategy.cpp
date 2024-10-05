#include <zaf/control/internal/list_control/list_control_item_height_strategy.h>

namespace zaf::internal {

void ListControlItemHeightStrategy::Initialize(
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    item_count_ = data_source.GetDataCount();
}


void ListControlItemHeightStrategy::OnItemAdd(
    const ListDataAddedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    item_count_ += event_info.count;
}


void ListControlItemHeightStrategy::OnItemUpdate(
    const ListDataUpdatedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

}


void ListControlItemHeightStrategy::OnItemRemove(const ListDataRemovedInfo& event_info) {

    item_count_ -= event_info.count;
}

}