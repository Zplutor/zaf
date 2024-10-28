#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_control_parts_context.h>

namespace zaf::internal {

void ListSingleSelectionStrategy::ChangeSelectionOnMouseDown(std::size_t item_index) {
    Parts().SelectionStore().ReplaceSelection(item_index, 1);
}


void ListSingleSelectionStrategy::ChangeSelectionOnMouseMove(std::size_t item_index) {
    Parts().SelectionStore().ReplaceSelection(item_index, 1);
}


void ListSingleSelectionStrategy::ChangeSelectionOnKeyDown(std::size_t item_index) {
    Parts().SelectionStore().ReplaceSelection(item_index, 1);
}

}