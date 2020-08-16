#pragma once

#include <vector>
#include <zaf/base/define.h>

namespace zaf::internal {

class TreeData {
public:
    void AddChildren(const IndexPath& parent, std::size_t count);
    void RemoveChildren(const IndexPath& parent, std::size_t count);

    std::size_t GetNodeCount() const;

    IndexPath GetIndexPathAtIndex(std::size_t index);

public:
    std::map<IndexPath, std::size_t> node_child_count_map_;
};

}