#pragma once

#include <vector>
#include <zaf/base/define.h>

namespace zaf::internal {

class TreeData {
public:
    IndexPath GetIndexPathAtIndex(std::size_t index);
    std::size_t GetIndexAtIndexPath(const IndexPath& path);

    std::size_t GetNodeCount() const;

    void AddChildren(const IndexPath& parent, std::size_t index, std::size_t count);
    void RemoveChildren(const IndexPath& parent, std::size_t index, std::size_t count);

public: //For unitest.
    std::vector<std::pair<IndexPath, std::size_t>> node_child_count_pairs;
};

}