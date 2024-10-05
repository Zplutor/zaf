#include <zaf/control/list_data_source.h>

namespace zaf {
namespace {

template<typename T>
void RaiseEvent(Subject<T>& subject, std::size_t index, std::size_t count) {

    if (count == 0) {
        return;
    }

    T event_info;
    event_info.index = index;
    event_info.count = count;
    subject.AsObserver().OnNext(event_info);
}

}

void ListDataSource::NotifyDataAdded(std::size_t index, std::size_t count) {
    RaiseEvent(data_added_event_, index, count);
}


void ListDataSource::NotifyDataRemoved(std::size_t index, std::size_t count) {
    RaiseEvent(data_removed_event_, index, count);
}


void ListDataSource::NotifyDataUpdated(std::size_t index, std::size_t count) {
    RaiseEvent(data_updated_event_, index, count);
}

}