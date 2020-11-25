#pragma once

#include <optional>
#include <vector>
#include <zaf/base/define.h>

namespace zaf::internal {

class TreeIndexMapping {
public:
    TreeIndexMapping() = default;

    TreeIndexMapping(const TreeIndexMapping&) = delete;
    TreeIndexMapping& operator=(const TreeIndexMapping&) = delete;

    IndexPath GetIndexPathAtIndex(std::size_t index) const;
    std::size_t GetIndexAtIndexPath(const IndexPath& path) const;

    std::size_t GetNodeCount() const;
    std::optional<std::size_t> GetChildCount(const IndexPath& parent) const;
    std::optional<std::size_t> GetChildCountRecursively(const IndexPath& parent) const;

    void AddChildren(const IndexPath& parent, std::size_t index, std::size_t count);
    std::size_t RemoveChildren(const IndexPath& parent, std::size_t index, std::size_t count);

    std::size_t RemoveAllChildrenRecursively(const IndexPath& parent);

    void Clear();

public: //For unitest.
    std::vector<std::pair<IndexPath, std::size_t>> node_child_count_pairs;
};

}