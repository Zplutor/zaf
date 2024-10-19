#pragma once

#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf {
namespace internal {

class ListSingleSelectionStrategy : public ListSelectionStrategy {
public:
    using ListSelectionStrategy::ListSelectionStrategy;

    void BeginChangingSelectionByMouseDown(const Point& position) override;

    void ChangeSelectionByMouseMove(const Point& position) override;

    void EndChangingSelectionByMouseUp(const Point& position) override;

    bool ChangeSelectionByKeyDown(const KeyMessage& message) override;

private:
    void SelectItemWithMouseEvent(const Point& position);

private:
    std::optional<std::size_t> mouse_selected_index_;
};

}
}