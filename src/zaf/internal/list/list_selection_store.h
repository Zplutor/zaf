#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/range_set.h>
#include <zaf/internal/list/list_selection_change_reason.h>
#include <zaf/rx/subject/subject.h>

namespace zaf::internal {

struct ListSelectionStoreChangedInfo {
    ListSelectionChangeReason reason{};
    Range changed_range;
};

/**
A passive object that stores selected indexes in a list control.
*/
class ListSelectionStore : NonCopyableNonMovable {
public:
    /**
    Replaces the existing selection with the specified range.

    @return
        Whether the selection is changed.
    */
    bool ReplaceSelection(std::size_t index, std::size_t count);

    /**
    Reverts selection at the specified index.
    */
    void RevertSelection(std::size_t index);

    /**
    Adds new selection to the existing selection with the specified range.

    @return
        Whether the selection is changed.
    */
    bool AddSelection(std::size_t index, std::size_t count);

    /**
    Removes selection from the existing selection with the specified range.

    @return
        Whether the selection is changed.
    */
    bool RemoveSelection(std::size_t index, std::size_t count);
    
    bool AdjustSelectionByAddingIndexes(std::size_t add_index, std::size_t add_count);
    bool AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count);
    bool AdjustSelectionByMovingIndex(std::size_t old_index, std::size_t new_index);

    bool IsIndexSelected(std::size_t index) const {
        return range_set_.ContainsIndex(index);
    }

    std::optional<std::size_t> GetFirstSelectedIndex() const noexcept {

        if (!range_set_.IsEmpty()) {
            return range_set_.begin()->index;
        }
        else {
            return std::nullopt;
        }
    }

    std::size_t GetAllSelectedCount() const;
    std::vector<std::size_t> GetAllSelectedIndexes() const;

    rx::Observable<ListSelectionStoreChangedInfo> ChangedEvent() const {
        return changed_event_.AsObservable();
    }

private:
    RangeSet range_set_;
    zaf::rx::Subject<ListSelectionStoreChangedInfo> changed_event_;
};

}