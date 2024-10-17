#pragma once

#include <set>
#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf {
namespace internal {

class ListExtendedMultipleSelectionStrategy : public ListSelectionStrategy {
public:
    ListExtendedMultipleSelectionStrategy();

    void BeginChangingSelectionByMouseDown(
        const Point& position, 
        const MouseMessage& message) override;

    void ChangeSelectionByMouseMove(
        const Point& position, 
        const MouseMessage& message) override;

    void EndChangingSelectionByMouseUp(
        const Point& position,
        const MouseMessage& message) override;

    bool ChangeSelectionByKeyDown(const KeyMessage& message) override;

private:
    void SelectItemsByMouseEvent(const Point& position, bool is_mouse_moving);
    void SelectItemsBetweenFocusedAndSpecified(std::size_t index);
    void SelectItemsByMouseEventWithControlKey(std::size_t current_index, bool is_mouse_moving);
    void RecoverSelectionStatesNotInRange(std::size_t index, std::size_t count);
    void RecordSelectionStatesInRange(std::size_t index, std::size_t count);

private:
    std::optional<std::size_t> focused_index_;
    bool is_focused_index_orginally_selected_;
    
    std::size_t orginally_recorded_index_;
    std::size_t orginally_recorded_count_;
    std::set<std::size_t> orginally_selected_indexes_;

    std::optional<std::size_t> last_focused_index_with_shift_key_;

    ListSelectionChangeReason selection_change_reason_{ ListSelectionChangeReason::AddSelection };
    std::size_t selection_change_index_{};
    std::size_t selection_change_count_{};
};

}
}