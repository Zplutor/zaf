#pragma once

#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf {
namespace internal {

class ListSimpleMultipleSelectionStrategy : public ListSelectionStrategy {
public:
    using ListSelectionStrategy::ListSelectionStrategy;

    void BeginChangingSelectionByMouseDown(const Point& position) override;

    void ChangeSelectionByMouseMove(const Point& position) override;

    void EndChangingSelectionByMouseUp(const Point& position) override;

    bool ChangeSelectionByKeyDown(const KeyMessage& message) override;

private:
    std::optional<std::size_t> mouse_selected_index_;
    bool is_mouse_selected_index_selected_{};
};

}
}