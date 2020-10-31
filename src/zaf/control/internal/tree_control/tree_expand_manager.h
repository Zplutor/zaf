#pragma once

#include <zaf/base/define.h>
#include <zaf/control/internal/tree_control/tree_data.h>
#include <zaf/object/object.h>

namespace zaf::internal {

class ExpandNodeInfo {
public:
    std::shared_ptr<Object> node_data;
    std::size_t children_count{};
};

class TreeExpandManager {
public:
    TreeExpandManager() = default;
    ~TreeExpandManager() = default;

    TreeExpandManager(const TreeExpandManager&) = delete;
    TreeExpandManager& operator=(const TreeExpandManager&) = delete;

    std::size_t Expand(
        const IndexPath& index_path, 
        const ExpandNodeInfo& expand_node_info,
        TreeData& tree_data);

    void Collapse(const IndexPath& index_path);

    void Clear();

    bool IsNodeExpanded(const IndexPath& index_path) const;

    const ExpandNodeInfo* GetNodeInfo(const IndexPath& index_path) const;

private:
    std::map<IndexPath, ExpandNodeInfo> expanded_nodes_;
};

}