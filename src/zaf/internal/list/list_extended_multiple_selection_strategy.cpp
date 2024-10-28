#include <zaf/internal/list/list_extended_multiple_selection_strategy.h>
#include <algorithm>
#include <zaf/input/keyboard.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/internal/list/list_item_height_manager.h>
#include <zaf/control/list_control.h>

#undef max
#undef min

namespace zaf {
namespace internal {

inline void MakeSelectionRange(
    std::size_t index1, 
    std::size_t index2, 
    std::size_t& select_index, 
    std::size_t& select_count) {

    select_index = std::min(index1, index2);
    select_count = std::max(index1, index2) - select_index + 1;
}


void ListExtendedMultipleSelectionStrategy::ChangeSelectionOnMouseDown(std::size_t item_index) {
    SelectItemsByMouseEvent(item_index, false);
}


void ListExtendedMultipleSelectionStrategy::ChangeSelectionOnMouseMove(std::size_t item_index) {
    SelectItemsByMouseEvent(item_index, true);
}


void ListExtendedMultipleSelectionStrategy::SelectItemsByMouseEvent(
    std::size_t item_index,
    bool is_mouse_moving) {

    bool is_pressing_shift_key = Keyboard::IsShiftDown();
    bool is_pressing_control_key = Keyboard::IsCtrlDown();

    //Cannot press shift and control at the same time.
    if (is_pressing_shift_key && is_pressing_control_key) {
        return;
    }

    if (is_pressing_shift_key) {
        SelectItemsBetweenAnchorAndSpecified(item_index);
    }
    else if (is_pressing_control_key) {
        SelectItemsByMouseEventWithControlKey(item_index, is_mouse_moving);
    }
    else {

        if (! is_mouse_moving) {
            anchor_index_ = item_index;
        }

        SelectItemsBetweenAnchorAndSpecified(item_index);
    }
}


void ListExtendedMultipleSelectionStrategy::SelectItemsBetweenAnchorAndSpecified(
    std::size_t index) {

    if (!anchor_index_) {
        return;
    }

    std::size_t select_index = 0;
    std::size_t select_count = 0;
    MakeSelectionRange(index, *anchor_index_, select_index, select_count);

    Parts().SelectionStore().ReplaceSelection(select_index, select_count);
}


void ListExtendedMultipleSelectionStrategy::SelectItemsByMouseEventWithControlKey(
    std::size_t current_index, 
    bool is_mouse_moving) {

    if (! is_mouse_moving) {

        anchor_index_ = current_index;

        originally_recorded_index_ = 0;
        originally_recorded_count_ = 0;
        originally_selected_indexes_.clear();
    }

    if (!anchor_index_) {
        return;
    }

    std::size_t select_index = 0;
    std::size_t select_count = 0;
    MakeSelectionRange(current_index, *anchor_index_, select_index, select_count);

    //Recover originally selection states not in current selection range.
    RecoverSelectionStatesNotInRange(select_index, select_count);

    //Record originally selection states in current selection range.
    RecordSelectionStatesInRange(select_index, select_count);

    if (! is_mouse_moving) {
        is_focused_index_originally_selected_ = 
            originally_selected_indexes_.find(current_index) != originally_selected_indexes_.end();
    }

    //Add or remove newly selection.
    if (is_focused_index_originally_selected_) {
        Parts().SelectionStore().RemoveSelection(select_index, select_count);
    }
    else {
        Parts().SelectionStore().AddSelection(select_index, select_count);
    }
}


void ListExtendedMultipleSelectionStrategy::RecoverSelectionStatesNotInRange(
    std::size_t index,
    std::size_t count) {

    auto& selection_store = Parts().SelectionStore();

    for (std::size_t current_index = originally_recorded_index_;
         current_index != originally_recorded_index_ + originally_recorded_count_;
         ++current_index) {

        if ((current_index < index) || (index + count <= current_index)) {

            if (originally_selected_indexes_.find(current_index) == originally_selected_indexes_.end()) {
                selection_store.RemoveSelection(current_index, 1);
            }
            else {
                selection_store.AddSelection(current_index, 1);
            }
        }
    }
}


void ListExtendedMultipleSelectionStrategy::RecordSelectionStatesInRange(
    std::size_t index,
    std::size_t count) {

    auto& selection_store = Parts().SelectionStore();

    for (std::size_t current_index = index; current_index != index + count; ++current_index) {

        if ((current_index < originally_recorded_index_) || 
            (originally_recorded_index_ + originally_recorded_count_ <= current_index)) {

            bool is_selected = selection_store.IsIndexSelected(current_index);
            if (is_selected) {
                originally_selected_indexes_.insert(current_index);
            }
        }
    }

    originally_recorded_index_ = index;
    originally_recorded_count_ = count;

    auto iterator = originally_selected_indexes_.begin();
    while (iterator != originally_selected_indexes_.end()) {

        if ((*iterator < index) || (index + count <= *iterator)) {
            iterator = originally_selected_indexes_.erase(iterator);
        }
        else {
            ++iterator;
        }
    }
}


void ListExtendedMultipleSelectionStrategy::ChangeSelectionOnMouseUp(std::size_t) {

    originally_recorded_index_ = 0;
    originally_recorded_count_ = 0;
    originally_selected_indexes_.clear();
}


void ListExtendedMultipleSelectionStrategy::ChangeSelectionOnKeyDown(std::size_t item_index) {

    bool is_pressing_shift_key = Keyboard::IsShiftDown();

    std::optional<std::size_t> previous_index;
    if (is_pressing_shift_key) {
        previous_index = last_anchor_index_with_shift_key_;
    }

    if (!previous_index) {
        previous_index = anchor_index_;
    }

    std::size_t new_index = item_index;

    std::size_t select_range_index = 0;
    std::size_t select_range_count = 0;

    if (is_pressing_shift_key && anchor_index_) {

        MakeSelectionRange(*anchor_index_, new_index, select_range_index, select_range_count);
    }
    else {

        select_range_index = new_index;
        select_range_count = 1;

        anchor_index_ = new_index;
    }

    last_anchor_index_with_shift_key_ = new_index;

    Parts().SelectionStore().ReplaceSelection(select_range_index, select_range_count);
}

}
}