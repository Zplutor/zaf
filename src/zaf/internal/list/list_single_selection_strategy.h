#pragma once

#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf::internal {

class ListSingleSelectionStrategy : public ListSelectionStrategy {
public:
    using ListSelectionStrategy::ListSelectionStrategy;

    void ChangeSelectionOnMouseDown(std::size_t item_index) override;
    void ChangeSelectionOnMouseMove(std::size_t item_index) override;
    void ChangeSelectionOnKeyDown(std::size_t item_index) override;
};

}
