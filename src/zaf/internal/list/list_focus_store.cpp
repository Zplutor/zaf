#include <zaf/internal/list/list_focus_store.h>

namespace zaf::internal {

std::optional<std::size_t> ListFocusStore::Index() const noexcept {

    if (focused_index_.IsEmpty()) {
        return std::nullopt;
    }

    return focused_index_.begin()->index;
}


void ListFocusStore::SetIndex(std::optional<std::size_t> index) {

    focused_index_.Clear();
    if (index) {
        focused_index_.AddRange(Range{ *index, 1 });
    }
}


void ListFocusStore::AddRange(const Range& added_range) {
    focused_index_.InsertSpan(added_range);
}


void ListFocusStore::RemoveRange(const Range& removed_range) {
    focused_index_.EraseSpan(removed_range);
}


void ListFocusStore::MoveIndex(std::size_t from, std::size_t to) {

    if (focused_index_.IsEmpty()) {
        return;
    }

    if (from == to) {
        return;
    }

    auto focused_index = Index();
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
}

}