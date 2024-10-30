#pragma once

#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/range_set.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

struct ListFocusStoreChangedInfo {
    std::optional<std::size_t> focused_index;
};

class ListFocusStore : public ListControlPartsBased {
public:
    using ListControlPartsBased::ListControlPartsBased;

    std::optional<std::size_t> FocusedIndex() const;
    void SetFocusedIndex(std::optional<std::size_t> index);

    void AdjustFocusedIndexByAddingIndex(const Range& added_range);
    void AdjustFocusedIndexByRemovingIndex(const Range& removed_range);
    void AdjustFocusedIndexByMovingIndex(std::size_t from, std::size_t to);

    zaf::Observable<ListFocusStoreChangedInfo> ChangedEvent() const {
        return changed_event_.AsObservable();
    }

private:
    void NotifyChanged();

private:
    //There is only one focused index actually, but we use RangSet to represent it to utilize its 
    //index calculation abilities.
    RangeSet focused_index_;

    zaf::Subject<ListFocusStoreChangedInfo> changed_event_;
};

}