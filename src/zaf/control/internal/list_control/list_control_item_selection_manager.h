#pragma once

#include <optional>
#include <zaf/control/internal/range_manager.h>

namespace zaf {
namespace internal {

class ListControlItemSelectionManager {
public:
    ListControlItemSelectionManager() : 
        range_manager_(std::bind(&ListControlItemSelectionManager::RangeChangeNotification, this)) {

    }

    void ReplaceSelection(std::size_t index, std::size_t count) {

        range_manager_.RemoveAllRanges();
        range_manager_.AddRange(index, count);
    }

    //Reverts selection at index, and returns new selection state.
    bool RevertSelection(std::size_t index) {

        if (range_manager_.IsPositionInRange(index)) {
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
        range_changed_ = false;
        range_manager_.ExpandRanges(add_index, add_count);
        return range_changed_;
    }

    bool AdjustSelectionByRemovingIndexes(std::size_t remove_index, std::size_t remove_count) {
        range_changed_ = false;
        range_manager_.NarrowRanges(remove_index, remove_count);
        return range_changed_;
    }

    bool IsIndexSelected(std::size_t index) const {
        return range_manager_.IsPositionInRange(index);
    }

    std::optional<std::size_t> GetFirstSelectedIndex() const {

        if (range_manager_.GetRangeCount() != 0) {
            return range_manager_.GetRangeAtIndex(0).index;
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
    void RangeChangeNotification() {
        range_changed_ = true;
    }

private:
    RangeManager range_manager_;
    bool range_changed_ = false;
};

}
}