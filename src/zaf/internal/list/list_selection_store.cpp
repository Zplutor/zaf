#include <zaf/internal/list/list_selection_store.h>

namespace zaf {
namespace internal {

void ListSelectionStore::ReplaceSelection(std::size_t index, std::size_t count) {

    range_set_.Clear();
    range_set_.AddRange({ index, count });

    changed_event_.AsObserver().OnNext({
        .reason = ListSelectionChangeReason::ReplaceSelection,
        .changed_range = Range{ index, count },
    });
}


bool ListSelectionStore::RevertSelection(std::size_t index) {

    if (range_set_.ContainsIndex(index)) {
        RemoveSelection(index, 1);
        return false;
    }
    else {
        AddSelection(index, 1);
        return true;
    }
}


void ListSelectionStore::AddSelection(std::size_t index, std::size_t count) {

    range_set_.AddRange({ index, count });

    changed_event_.AsObserver().OnNext({
        .reason = ListSelectionChangeReason::AddSelection,
        .changed_range = Range{ index, count },
    });
}


void ListSelectionStore::RemoveSelection(std::size_t index, std::size_t count) {

    range_set_.RemoveRange({ index, count });

    changed_event_.AsObserver().OnNext({
        .reason = ListSelectionChangeReason::RemoveSelection,
        .changed_range = Range{ index, count },
    });
}


bool ListSelectionStore::AdjustSelectionByAddingIndexes(
    std::size_t add_index, 
    std::size_t add_count) {

    if (add_count == 0) {
        return false;
    }

    range_set_.InsertSpan(Range{ add_index, add_count });

    changed_event_.AsObserver().OnNext({
        .reason = ListSelectionChangeReason::ItemChange,
    });
    return true;
}


bool ListSelectionStore::AdjustSelectionByRemovingIndexes(
    std::size_t remove_index, 
    std::size_t remove_count) {

    if (remove_count == 0) {
        return false;
    }

    range_set_.EraseSpan(Range{ remove_index, remove_count });
    changed_event_.AsObserver().OnNext({
        .reason = ListSelectionChangeReason::ItemChange,
    });
    return true;
}


bool ListSelectionStore::AdjustSelectionByMovingIndex(
    std::size_t old_index, 
    std::size_t new_index) {

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

    changed_event_.AsObserver().OnNext({
        .reason = ListSelectionChangeReason::ItemChange,
    });
    return true;
}


std::size_t ListSelectionStore::GetAllSelectedCount() const {

    std::size_t count = 0;
    for (const auto& each_range : range_set_) {
        count += each_range.length;
    }
    return count;
}


std::vector<std::size_t> ListSelectionStore::GetAllSelectedIndexes() const {

    std::vector<std::size_t> indexes;
    for (const auto& each_range : range_set_) {
        for (auto index : each_range) {
            indexes.push_back(index);
        }
    }
    return indexes;
}

}
}