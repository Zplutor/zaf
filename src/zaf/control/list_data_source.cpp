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

void ListDataSource::NotifyDataAdd(std::size_t index, std::size_t count) {
    RaiseEvent(data_add_event_, index, count);
}


void ListDataSource::NotifyDataRemove(std::size_t index, std::size_t count) {
    RaiseEvent(data_remove_event_, index, count);
}


void ListDataSource::NotifyDataUpdate(std::size_t index, std::size_t count) {
    RaiseEvent(data_update_event_, index, count);
}

}