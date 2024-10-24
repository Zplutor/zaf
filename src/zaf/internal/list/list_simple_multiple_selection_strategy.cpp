#include <zaf/internal/list/list_simple_multiple_selection_strategy.h>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/control/list_control.h>

namespace zaf::internal {

void ListSimpleMultipleSelectionStrategy::ChangeSelectionOnMouseDown(std::size_t item_index) {
    Parts().SelectionStore().RevertSelection(item_index);
}

}
