#include <zaf/internal/list/list_selection_manager.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_extended_multiple_selection_strategy.h>
#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_simple_multiple_selection_strategy.h>

namespace zaf::internal {

zaf::SelectionMode ListSelectionManager::SelectionMode() const noexcept {
    return selection_model_;
}


void ListSelectionManager::SetSelectionMode(zaf::SelectionMode mode) {

    selection_model_ = mode;

    selection_strategy_ = [this]() -> std::unique_ptr<ListSelectionStrategy> {

        auto context = &Context();

        switch (selection_model_) {
        case SelectionMode::Single:
            return std::make_unique<ListSingleSelectionStrategy>(context);

        case SelectionMode::SimpleMultiple:
            return std::make_unique<ListSimpleMultipleSelectionStrategy>(context);

        case SelectionMode::ExtendedMultiple:
            return std::make_unique<ListExtendedMultipleSelectionStrategy>(context);

        default:
            return std::make_unique<ListNoSelectionStrategy>(context);
        }
    }();
}


ListSelectionStrategy& ListSelectionManager::SelectionStrategy() const noexcept {
    return *selection_strategy_;
}

}