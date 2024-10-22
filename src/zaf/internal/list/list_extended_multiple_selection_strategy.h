#pragma once

#include <set>
#include <zaf/internal/list/list_control_core.h>
#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf {
namespace internal {

class ListExtendedMultipleSelectionStrategy : public ListSelectionStrategy {
public:
    using ListSelectionStrategy::ListSelectionStrategy;

    void ChangeSelectionOnMouseDown(std::size_t item_index) override;
    void ChangeSelectionOnMouseMove(std::size_t item_index) override;
    void ChangeSelectionOnMouseUp(std::size_t item_index) override;

    std::optional<std::size_t> ChangeSelectionOnKeyDown(const KeyMessage& message) override;

private:
    void SelectItemsByMouseEvent(std::size_t item_index, bool is_mouse_moving);
    void SelectItemsBetweenFocusedAndSpecified(std::size_t index);
    void SelectItemsByMouseEventWithControlKey(std::size_t current_index, bool is_mouse_moving);
    void RecoverSelectionStatesNotInRange(std::size_t index, std::size_t count);
    void RecordSelectionStatesInRange(std::size_t index, std::size_t count);

private:
    std::optional<std::size_t> focused_index_;
    bool is_focused_index_orginally_selected_{};
    
    std::size_t orginally_recorded_index_{};
    std::size_t orginally_recorded_count_{};
    std::set<std::size_t> orginally_selected_indexes_;

    std::optional<std::size_t> last_focused_index_with_shift_key_;
};

}
}