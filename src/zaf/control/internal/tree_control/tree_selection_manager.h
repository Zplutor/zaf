#pragma once

#include <map>
#include <unordered_map>
#include <zaf/base/define.h>
#include <zaf/control/internal/tree_control/tree_data_helpers.h>
#include <zaf/object/object.h>

namespace zaf::internal {

class TreeSelectionManager {
public:
    TreeSelectionManager() = default;
    ~TreeSelectionManager() = default;

    TreeSelectionManager(const TreeSelectionManager&) = delete;
    TreeSelectionManager& operator=(const TreeSelectionManager&) = delete;

    bool IsDataSelected(const std::shared_ptr<Object>& data) const;
    std::vector<std::shared_ptr<Object>> GetAllSelectedData() const;
    std::shared_ptr<Object> GetFirstSelectedData() const;

    void AddSelectedData(const std::vector<std::shared_ptr<Object>>& data_list);
    void RemoveSelectedData(const std::vector<std::shared_ptr<Object>>& data_list);
    void Clear();

private:
    using TreeDataMap = std::unordered_map<
        std::shared_ptr<Object>, 
        std::size_t, 
        TreeDataHash, 
        TreeDataEqual>;

private:
    TreeDataMap selected_data_map_;
    std::size_t order_seed_{};
};

}