#include <zaf/control/list_data_source.h>

namespace zaf {

void ListDataSource::NotifyDataAdd(std::size_t index, std::size_t count) {
    if (count != 0) {
        data_add_event_.Trigger(index, count);
    }
}


void ListDataSource::NotifyDataRemove(std::size_t index, std::size_t count) {
    if (count != 0) {
        data_remove_event_.Trigger(index, count);
    }
}


void ListDataSource::NotifyDataUpdate(std::size_t index, std::size_t count) {
    if (count != 0) {
        data_update_event_.Trigger(index, count);
    }
}

}