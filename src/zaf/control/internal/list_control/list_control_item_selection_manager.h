#pragma once

#include <optional>
#include <zaf/control/internal/range_manager.h>

namespace zaf {
namespace internal {

class ListControlItemSelectionManager {
public:
    ListControlItemSelectionManager() {

    }

    void ReplaceSelection(std::size_t index, std::size_t count) {

        range_manager_.Clear();
        range_manager_.AddRange(index, count);
    }

    //Reverts selection at index, and returns new selection state.
    bool RevertSelection(std::size_t index) {

        if (range_manager_.IsIndexInRange(index)) {
            range_manager_.RemoveRange(index, 1);
            return false;
        }
        else {
            range_manager_.AddRange(index, 1);
            return true;
        }
    }

    void AddSelection(std::size_t index, std::size_t count) {
        range_manager_.AddRange(index, count);
    }

    void RemoveSelection(std::size_t index, std::size_t count) {
        range_manager_.RemoveRange(index, count);
    }
    
    bool AdjustSelectionByAddingIndexes(std::size_t add_index, std::size_t add_count) {
        return range_manager_.InsertSpan(Range{ add_index, add_count });
    }

    bool AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count) {
        return range_manager_.EraseSpan(Range{ remove_index, remove_count });
    }

    bool IsIndexSelected(std::size_t index) const {
        return range_manager_.IsIndexInRange(index);
    }

    std::optional<std::size_t> GetFirstSelectedIndex() const {

        if (range_manager_.RangeCount() != 0) {
            return range_manager_.GetRange(0).index;
        }
        else {
            return std::nullopt;
        }
    }

    std::size_t GetAllSelectedCount() const;
    std::vector<std::size_t> GetAllSelectedIndexes() const;

    ListControlItemSelectionManager(ListControlItemSelectionManager&) = delete;
    ListControlItemSelectionManager& operator=(ListControlItemSelectionManager&) = delete;

private:
    RangeManager range_manager_;
};

}
}