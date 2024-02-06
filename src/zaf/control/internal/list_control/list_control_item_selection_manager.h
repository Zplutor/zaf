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
        return range_set_.InsertSpan(Range{ add_index, add_count });
    }

    bool AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count) {
        return range_set_.EraseSpan(Range{ remove_index, remove_count });
    }

    bool IsIndexSelected(std::size_t index) const {
        return range_set_.ContainsIndex(index);
    }

    std::optional<std::size_t> GetFirstSelectedIndex() const {

        if (!range_set_.IsEmpty()) {
            return range_set_[0].index;
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