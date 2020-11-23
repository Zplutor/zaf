#include <zaf/control/internal/tree_control/tree_selection_manager.h>
#include <zaf/base/container/utility/find.h>
#include <zaf/base/container/utility/range.h>

namespace zaf::internal {

bool TreeSelectionManager::IsChildIndexSelected(
    const std::shared_ptr<Object>& parent_data, 
    std::size_t index) const {

    auto range_manager = Find(data_range_managers_, parent_data);
    if (!range_manager) {
        return false;
    }

    return range_manager->IsPositionInRange(index);
}


TreeSelectionManager::ParentDataAndChildIndexes 
    TreeSelectionManager::GetAllSelectedChildIndexes() const {

    ParentDataAndChildIndexes result;

    for (const auto& each_pair : data_range_managers_) {

        std::vector<std::size_t> child_indexes;

        for (auto index : zaf::Range(0u, each_pair.second.GetRangeCount())) {

            auto range = each_pair.second.GetRangeAtIndex(index);
            for (auto child_index : zaf::Range(range.first, range.first + range.second)) {

                child_indexes.push_back(child_index);
            }
        }

        result.emplace_back(each_pair.first, std::move(child_indexes));
    }

    return result;
}


void TreeSelectionManager::AddSelection(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    if (count <= 0) {
        return;
    }

    auto& range_manager = data_range_managers_[parent_data];
    range_manager.AddRange(index, count);
}


void TreeSelectionManager::RemoveSelection(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    if (count <= 0) {
        return;
    }

    auto iterator = data_range_managers_.find(parent_data);
    if (iterator == data_range_managers_.end()) {
        return;
    }

    iterator->second.RemoveRange(index, count);

    if (iterator->second.GetRangeCount() == 0) {
        data_range_managers_.erase(iterator);
    }
}


void TreeSelectionManager::AddDataToSelection(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    auto range_manager = Find(data_range_managers_, parent_data);
    if (!range_manager) {
        return;
    }

    range_manager->ExpandRanges(index, count);
}


void TreeSelectionManager::RemoveDataFromSelection(
    const std::shared_ptr<Object>& parent_data,
    std::size_t index,
    std::size_t count) {

    auto range_manager = Find(data_range_managers_, parent_data);
    if (!range_manager) {
        return;
    }

    range_manager->NarrowRanges(index, count);
}


void TreeSelectionManager::Clear() {
    data_range_managers_.clear();
}

}