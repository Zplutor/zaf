#include <zaf/control/list_data_source.h>

namespace zaf {
namespace {

template<typename T>
void RaiseEvent(const Subject<T>& subject, std::size_t index, std::size_t count) {

    if (count == 0) {
        return;
    }

    T event_info{ index, count };
    subject.AsObserver().OnNext(event_info);
}

}

void ListDataSource::NotifyDataAdded(std::size_t index, std::size_t count) const {
    RaiseEvent(data_added_event_, index, count);
}


void ListDataSource::NotifyDataRemoved(std::size_t index, std::size_t count) const {
    RaiseEvent(data_removed_event_, index, count);
}


void ListDataSource::NotifyDataUpdated(std::size_t index, std::size_t count) const {
    RaiseEvent(data_updated_event_, index, count);
}

}