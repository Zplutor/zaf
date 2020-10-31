#pragma once

#include <set>
#include <zaf/base/define.h>
#include <zaf/object/object.h>

namespace zaf::internal {

class TreeSelectionManager {
public:
    TreeSelectionManager() = default;
    ~TreeSelectionManager() = default;

    TreeSelectionManager(const TreeSelectionManager&) = delete;
    TreeSelectionManager& operator=(const TreeSelectionManager&) = delete;

    bool IsIndexPathSelected(const IndexPath& index_path) const;
    std::vector<std::shared_ptr<Object>> GetAllSelectedNodeData() const;
    std::shared_ptr<Object> GetFirstSelectedNodeData() const;

    void AddSelectedNodes(const std::map<IndexPath, std::shared_ptr<Object>>& nodes);
    void RemoveSelectedNodes(const std::vector<IndexPath>& index_paths);
    void Clear();

private:
    std::map<IndexPath, std::shared_ptr<Object>> selected_nodes_;
};

}