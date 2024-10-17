#include <zaf/control/internal/list_control/list_control_item_height_strategy.h>

namespace zaf::internal {

void ListItemHeightStrategy::Initialize(
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    item_count_ = data_source.GetDataCount();
}


void ListItemHeightStrategy::OnDataAdded(
    const ListDataAddedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

    item_count_ += event_info.Count();
}


void ListItemHeightStrategy::OnDataUpdated(
    const ListDataUpdatedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

}


void ListItemHeightStrategy::OnDataMoved(
    const ListDataMovedInfo& event_info,
    ListDataSource& data_source,
    ListControlDelegate& delegate) {

}


void ListItemHeightStrategy::OnDataRemoved(const ListDataRemovedInfo& event_info) {

    item_count_ -= event_info.Count();
}

}