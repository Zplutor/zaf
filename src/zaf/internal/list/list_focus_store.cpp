#include <zaf/internal/list/list_focus_store.h>

namespace zaf::internal {

std::optional<std::size_t> ListFocusStore::FocusedIndex() const {

    if (focused_index_.IsEmpty()) {
        return std::nullopt;
    }

    return focused_index_.begin()->index;
}


void ListFocusStore::SetFocusedIndex(std::optional<std::size_t> index) {

    focused_index_.Clear();
    if (index) {
        focused_index_.AddRange(Range{ *index, 1 });
    }
    NotifyChanged();
}


void ListFocusStore::AdjustFocusedIndexByAddingIndex(const Range& added_range) {

    focused_index_.InsertSpan(added_range);
    NotifyChanged();
}


void ListFocusStore::AdjustFocusedIndexByRemovingIndex(const Range& removed_range) {

    focused_index_.RemoveRange(removed_range);
    NotifyChanged();
}


void ListFocusStore::NotifyChanged() {

    changed_event_.AsObserver().OnNext(ListFocusStoreChangedInfo{
        .focused_index = FocusedIndex(),
    });
}

}