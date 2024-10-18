#include <zaf/internal/list/list_focus_manager.h>

namespace zaf::internal {

void ListFocusManager::SetFocusedIndex(std::size_t index) {

    focused_index_.Clear();
    focused_index_.AddRange(Range{ index, 1 });
}


void ListFocusManager::AdjustFocusedIndexByAddingIndex(const Range& added_range) {
    focused_index_.InsertSpan(added_range);
}


void ListFocusManager::AdjustFocusedIndexByRemovingIndex(const Range& removed_range) {
    focused_index_.RemoveRange(removed_range);
}

}