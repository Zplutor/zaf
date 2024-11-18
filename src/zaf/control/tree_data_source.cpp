#include <zaf/control/tree_data_source.h>

namespace zaf {
namespace {

template<typename T>
void RaiseEvent(
    Subject<T>& subject, 
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    if (count == 0) {
        return;
    }

    T event_info;
    event_info.parent_data = parent_data;
    event_info.index = index;
    event_info.count = count;
    subject.AsObserver().OnNext(event_info);
}

}

const std::shared_ptr<TreeDataSource>& TreeDataSource::Empty() {
    static auto empty = std::make_shared<TreeDataSource>();
    return empty;
}


void TreeDataSource::NotifyDataAdd(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    RaiseEvent(data_add_event_, parent_data, index, count);
}


void TreeDataSource::NotifyDataRemove(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    RaiseEvent(data_remove_event_, parent_data, index, count);
}


void TreeDataSource::NotifyDataUpdate(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    RaiseEvent(data_update_event_, parent_data, index, count);
}

}