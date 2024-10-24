#include <zaf/internal/list/list_item_height_strategy.h>

namespace zaf::internal {

ListItemHeightStrategy::ListItemHeightStrategy(
    std::weak_ptr<ListDataSource> data_source, 
    std::weak_ptr<ListControlDelegate> delegate) 
    :
    data_source_(data_source),
    delegate_(delegate) {

}


void ListItemHeightStrategy::Initialize() {

}


void ListItemHeightStrategy::OnDataAdded(const ListDataAddedInfo& event_info) {

}


void ListItemHeightStrategy::OnDataUpdated(const ListDataUpdatedInfo& event_info) {

}


void ListItemHeightStrategy::OnDataMoved(const ListDataMovedInfo& event_info) {

}


void ListItemHeightStrategy::OnDataRemoved(const ListDataRemovedInfo& event_info) {

}

}