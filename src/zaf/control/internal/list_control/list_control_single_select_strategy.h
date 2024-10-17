#pragma once

#include <zaf/control/internal/list_control/list_control_select_strategy.h>

namespace zaf {
namespace internal {

class ListSingleSelectionStrategy : public ListSelectionStrategy {
public:
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
    void SelectItemWithMouseEvent(const Point& position);

private:
    std::optional<std::size_t> mouse_selected_index_;
};

}
}