#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/range_set.h>

namespace zaf::internal {

class ListFocusManager : NonCopyableNonMovable {
public:
    void SetFocusedIndex(std::size_t index);

    void AdjustFocusedIndexByAddingIndex(const Range& added_range);
    void AdjustFocusedIndexByRemovingIndex(const Range& removed_range);

private:
    RangeSet focused_index_;
};

}