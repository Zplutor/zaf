#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_control_part_context.h>
#include <zaf/internal/list/list_item_height_manager.h>

namespace zaf::internal {

void ListSingleSelectionStrategy::ChangeSelectionOnMouseDown(std::size_t item_index) {
    Context().SelectionStore().ReplaceSelection(item_index, 1);
}


void ListSingleSelectionStrategy::ChangeSelectionOnMouseMove(std::size_t item_index) {
    Context().SelectionStore().ReplaceSelection(item_index, 1);
}


std::optional<std::size_t> ListSingleSelectionStrategy::ChangeSelectionOnKeyDown(
    const KeyMessage& message) {

    auto& selection_store = Context().SelectionStore();

    auto previous_selected_index = selection_store.GetFirstSelectedIndex();
    std::size_t new_selected_index{};
    bool change_index = ChangeIndexByKeyDown(message, previous_selected_index, new_selected_index);
    if (!change_index) {
        return std::nullopt;
    }

    selection_store.ReplaceSelection(new_selected_index, 1);
    return new_selected_index;
}

}