#include <zaf/control/internal/tree_control/tree_selection_manager.h>
#include <zaf/base/container/utility/contain.h>
#include <zaf/base/container/utility/conversion.h>
#include <zaf/base/container/utility/Sort.h>
#include <zaf/control/internal/tree_control/utility.h>

namespace zaf::internal {

bool TreeSelectionManager::IsDataSelected(const std::shared_ptr<Object>& data) const {
    return Contain(selected_data_map_, data);
}


std::vector<std::shared_ptr<Object>> TreeSelectionManager::GetAllSelectedData() const {

    std::vector<std::pair<std::shared_ptr<Object>, std::size_t>> sorted_data_list;
    for (const auto& each_pair : selected_data_map_) {
        sorted_data_list.push_back(each_pair);
    }

    Sort(sorted_data_list, [](const auto& pair1, const auto& pair2) {
        return pair1.second < pair2.second;
    });

    std::vector<std::shared_ptr<Object>> result;
    for (const auto& each_pair : sorted_data_list) {
        result.push_back(each_pair.first);
    }

    return result;
}


std::shared_ptr<Object> TreeSelectionManager::GetFirstSelectedData() const {

    //Call GetAllSelectedData does more unnecessary work, but may be acceptable.
    auto all_selected_data = GetAllSelectedData();
    if (!all_selected_data.empty()) {
        return all_selected_data.front();
    }
    return nullptr;
}


void TreeSelectionManager::AddSelectedData(const std::vector<std::shared_ptr<Object>>& data_list) {

    for (const auto& each_data : data_list) {

        std::size_t order = ++order_seed_;
        selected_data_map_[each_data] = order;
    }
}


void TreeSelectionManager::RemoveSelectedData(
    const std::vector<std::shared_ptr<Object>>& data_list) {

    for (const auto& each_data : data_list) {

        auto iterator = selected_data_map_.find(each_data);
        if (iterator != selected_data_map_.end()) {
            selected_data_map_.erase(iterator);
        }
    }

    if (selected_data_map_.empty()) {
        order_seed_ = 0;
    }
}


void TreeSelectionManager::Clear() {
    selected_data_map_.clear();
    order_seed_ = 0;
}

}