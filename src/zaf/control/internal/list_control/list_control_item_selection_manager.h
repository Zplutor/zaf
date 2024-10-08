#pragma once

#include <optional>
#include <zaf/base/non_copyable.h>
#include <zaf/control/internal/range_set.h>

namespace zaf {
namespace internal {

class ListControlItemSelectionManager : NonCopyableNonMovable {
public:
    ListControlItemSelectionManager() {

    }

    void ReplaceSelection(std::size_t index, std::size_t count) {

        range_set_.Clear();
        range_set_.AddRange({ index, count });
    }

    //Reverts selection at index, and returns new selection state.
    bool RevertSelection(std::size_t index) {

        if (range_set_.ContainsIndex(index)) {
            range_set_.RemoveRange({ index, 1 });
            return false;
        }
        else {
            range_set_.AddRange({ index, 1 });
            return true;
        }
    }

    void AddSelection(std::size_t index, std::size_t count) {
        range_set_.AddRange({ index, count });
    }

    void RemoveSelection(std::size_t index, std::size_t count) {
        range_set_.RemoveRange({ index, count });
    }
    
    bool AdjustSelectionByAddingIndexes(std::size_t add_index, std::size_t add_count) {
        if (add_count > 0) {
            range_set_.InsertSpan(Range{ add_index, add_count });
            return true;
        }
        return false;
    }

    bool AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count) {
        if (remove_count > 0) {
            range_set_.EraseSpan(Range{ remove_index, remove_count });
            return true;
        }
        return false;
    }

    bool AdjustSelectionByMovingIndex(std::size_t old_index, std::size_t new_index) {
        if (old_index == new_index) {
            return false;
        }
        range_set_.EraseSpan(Range{ old_index, 1 });
        if (range_set_.IsEmpty()) {
            range_set_.AddRange(Range{ new_index, 1 });
        }
        else {
            range_set_.InsertSpan(Range{ new_index, 1 });
        }
        return true;
    }

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

private:
    RangeSet range_set_;
};

}
}