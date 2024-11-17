#pragma once

#include <zaf/control/list_selection_mode.h>
#include <zaf/internal/list/list_control_parts_based.h>
#include <zaf/internal/list/list_selection_change_reason.h>
#include <zaf/internal/list/list_selection_strategy.h>

namespace zaf::internal {

/**
An active object that modifies the selection indexes of a list control by different ways, such as 
mouse input and keyboard input.
*/
class ListSelectionManager : public ListControlPartsBased {
public:
    explicit ListSelectionManager(ListControlPartsContext* context);

    zaf::ListSelectionMode SelectionMode() const noexcept;
    void SetSelectionMode(zaf::ListSelectionMode mode);

    ListSelectionStrategy& SelectionStrategy() const noexcept;

    void SelectAllItems();
    void UnselectAllItems();

    void SelectItemAtIndex(std::size_t index);
    void UnselectItemAtIndex(std::size_t index);

private:
    void ReviseSelectionBySelectionMode();
    void ResetSelectionStrategy();

private:
    zaf::ListSelectionMode selection_mode_{ zaf::ListSelectionMode::Single };
    std::unique_ptr<ListSelectionStrategy> selection_strategy_;
};

}