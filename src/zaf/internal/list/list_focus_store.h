#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/range_set.h>

namespace zaf::internal {

class ListFocusStore : NonCopyableNonMovable {
public:
    std::optional<std::size_t> Index() const noexcept;
    void SetIndex(std::optional<std::size_t> index);

    void AddRange(const Range& added_range);
    void RemoveRange(const Range& removed_range);
    void MoveIndex(std::size_t from, std::size_t to);

private:
    /*
    There is only one focused item index actually, but we use RangSet to represent it to utilize
    its index calculation abilities.
    */
    RangeSet focused_index_;
};

}