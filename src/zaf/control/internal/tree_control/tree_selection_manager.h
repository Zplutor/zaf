#pragma once

#include <set>
#include <zaf/base/define.h>

namespace zaf::internal {

class TreeSelectionManager {
public:
    TreeSelectionManager() = default;
    ~TreeSelectionManager() = default;

    TreeSelectionManager(const TreeSelectionManager&) = delete;
    TreeSelectionManager& operator=(const TreeSelectionManager&) = delete;

    bool IsIndexPathSelected(const IndexPath& index_path);

    void AddSelectedIndexPaths(const std::vector<IndexPath>& index_paths);
    void RemoveSelectedIndexPaths(const std::vector<IndexPath>& index_paths);
    void ReplaceSelectedIndexPahts(const std::vector<IndexPath>& index_paths);

private:
    std::set<IndexPath> selected_index_paths_;
};

}