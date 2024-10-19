#pragma once

#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_part.h>
#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf::internal {

/**
An active object that modifies the selection indexes of a list control by different ways, such as 
mouse input and keyboard input.
*/
class ListSelectionManager : public ListControlPart {
public:
    using ListControlPart::ListControlPart;

    zaf::SelectionMode SelectionMode() const noexcept;
    void SetSelectionMode(zaf::SelectionMode mode);

    ListSelectionStrategy& SelectionStrategy() const noexcept;

private:
    zaf::SelectionMode selection_model_{ zaf::SelectionMode::None };
    std::unique_ptr<ListSelectionStrategy> selection_strategy_;
};

}