#include <zaf/control/list_box_data_source.h>
#include <zaf/base/error/check.h>

namespace zaf {

void ListBoxDataSource::AddDataAtIndex(std::size_t index, const std::shared_ptr<Object>& data) {

    ZAF_EXPECT(0 <= index && index <= data_list_.size());

    data_list_.insert(std::next(data_list_.begin(), index), data);

    NotifyDataAdd(index, 1);
}


void ListBoxDataSource::RemoveDataAtIndex(std::size_t index) {

    ZAF_EXPECT(0 <= index && index < data_list_.size());

    data_list_.erase(std::next(data_list_.begin(), index));

    NotifyDataRemove(index, 1);
}


void ListBoxDataSource::RemoveAllData() {

    if (data_list_.empty()) {
        return;
    }

    auto item_count = data_list_.size();
    data_list_.clear();
    NotifyDataRemove(0, item_count);
}

}