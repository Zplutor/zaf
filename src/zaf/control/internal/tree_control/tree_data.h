#pragma once

#include <optional>
#include <vector>
#include <zaf/base/define.h>

namespace zaf::internal {

class TreeData {
public:
    IndexPath GetIndexPathAtIndex(std::size_t index) const;
    std::size_t GetIndexAtIndexPath(const IndexPath& path) const;

    std::size_t GetNodeCount() const;
    std::optional<std::size_t> GetChildrenCount(const IndexPath& parent) const;

    void AddChildren(const IndexPath& parent, std::size_t index, std::size_t count);
    std::size_t RemoveChildren(const IndexPath& parent, std::size_t index, std::size_t count);

    std::size_t RemoveAllChildrenRecursively(const IndexPath& parent);

public: //For unitest.
    std::vector<std::pair<IndexPath, std::size_t>> node_child_count_pairs;
};

}