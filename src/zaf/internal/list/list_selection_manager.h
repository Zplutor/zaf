#pragma once

#include <zaf/control/selection_mode.h>
#include <zaf/internal/list/list_control_part.h>
#include <zaf/internal/list/list_selection_strategy.h>
#include <zaf/rx/subject.h>

namespace zaf::internal {

enum class ListSelectionChangeReason {
    ItemChange,
    ReplaceSelection,
    AddSelection,
    RemoveSelection,
};

struct ListSelectionChangedInfo {
    ListSelectionChangeReason reason{};
    std::size_t index{};
    std::size_t count{};
};

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

    void SelectAllItems();
    void UnselectAllItems();

    zaf::Observable<ListSelectionChangedInfo> SelectionChangedEvent() const {
        return selection_changed_event_.AsObservable();
    }

private:
    zaf::SelectionMode selection_model_{ zaf::SelectionMode::None };
    std::unique_ptr<ListSelectionStrategy> selection_strategy_;
    zaf::Subject<ListSelectionChangedInfo> selection_changed_event_;
};

}