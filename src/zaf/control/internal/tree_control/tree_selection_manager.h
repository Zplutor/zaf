#pragma once

#include <map>
#include <unordered_map>
#include <zaf/base/define.h>
#include <zaf/control/internal/range_manager.h>
#include <zaf/control/internal/tree_control/tree_data_helpers.h>
#include <zaf/object/object.h>

namespace zaf::internal {

class TreeSelectionManager {
public:
    using ParentDataAndChildIndexes = std::vector<
        std::pair<
            std::shared_ptr<Object>, 
            std::vector<std::size_t>
        >
    >;

public:
    TreeSelectionManager() = default;
    ~TreeSelectionManager() = default;

    TreeSelectionManager(const TreeSelectionManager&) = delete;
    TreeSelectionManager& operator=(const TreeSelectionManager&) = delete;

    bool IsChildIndexSelected(const std::shared_ptr<Object>& parent_data, std::size_t index) const;

    ParentDataAndChildIndexes GetAllSelectedChildIndexes() const;

    void AddSelection(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index, 
        std::size_t count);

    void RemoveSelection(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index,
        std::size_t count);

    void AddDataToSelection(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index, 
        std::size_t count);

    void RemoveDataFromSelection(
        const std::shared_ptr<Object>& parent_data,
        std::size_t index,
        std::size_t count);

    void Clear();

private:
    using TreeDataMap = std::unordered_map<
        std::shared_ptr<Object>, 
        RangeManager, 
        TreeDataHash, 
        TreeDataEqual>;

private:
    TreeDataMap data_range_managers_;
};

}