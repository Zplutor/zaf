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

    if (focused_index_.IsEmpty()) {
        return;
    }

    focused_index_.InsertSpan(added_range);
    NotifyChanged();
}


void ListFocusStore::AdjustFocusedIndexByRemovingIndex(const Range& removed_range) {

    if (focused_index_.IsEmpty()) {
        return;
    }

    focused_index_.EraseSpan(removed_range);
    NotifyChanged();
}


void ListFocusStore::AdjustFocusedIndexByMovingIndex(std::size_t from, std::size_t to) {

    if (focused_index_.IsEmpty()) {
        return;
    }

    if (from == to) {
        return;
    }

    auto focused_index = FocusedIndex();
    if (!focused_index) {
        return;
    }

    if (focused_index == from) {
        focused_index_.Clear();
        focused_index_.AddRange(Range{ to, 1 });
    }
    else {
        focused_index_.EraseSpan(Range{ from, 1 });
        focused_index_.InsertSpan(Range{ to, 1 });
    }
    NotifyChanged();
}


void ListFocusStore::NotifyChanged() {

    changed_event_.AsObserver().OnNext(ListFocusStoreChangedInfo{
        .focused_index = FocusedIndex(),
    });
}

}