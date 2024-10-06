#pragma once

#include <zaf/control/list_data_source.h>

namespace zaf {

class ListBoxDataSource : public ListDataSource {
public:
    void AddDataAtIndex(std::size_t index, const std::shared_ptr<Object>& data);
    void RemoveDataAtIndex(std::size_t index);
    void RemoveAllData();

    std::size_t GetDataCount() const override {
        return data_list_.size();
    }

    std::shared_ptr<Object> GetDataAtIndex(std::size_t index) const override {
        return data_list_[index];
    }

private:
    std::vector<std::shared_ptr<Object>> data_list_;
};

}