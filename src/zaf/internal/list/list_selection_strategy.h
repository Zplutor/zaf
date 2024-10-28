#pragma once

#include <zaf/internal/list/list_control_parts_based.h>

namespace zaf::internal {
    
class ListSelectionStrategy : public ListControlPartsBased {
public:
    using ListControlPartsBased::ListControlPartsBased;
    virtual ~ListSelectionStrategy() = default;

    virtual void ChangeSelectionOnMouseDown(std::size_t item_index) {

    }

    virtual void ChangeSelectionOnMouseMove(std::size_t item_index) {

    }

    virtual void ChangeSelectionOnMouseUp(std::size_t item_index) {

    }

    virtual void ChangeSelectionOnKeyDown(std::size_t item_index) {

    }
};


using ListNoSelectionStrategy = ListSelectionStrategy;

}