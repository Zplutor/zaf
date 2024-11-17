#include <zaf/internal/list/list_selection_manager.h>
#include <zaf/control/list_control.h>
#include <zaf/internal/list/list_control_parts_context.h>
#include <zaf/internal/list/list_extended_multiple_selection_strategy.h>
#include <zaf/internal/list/list_single_selection_strategy.h>
#include <zaf/internal/list/list_simple_multiple_selection_strategy.h>

namespace zaf::internal {

ListSelectionManager::ListSelectionManager(ListControlPartsContext* context) :
    ListControlPartsBased(context) {

    ResetSelectionStrategy();
}


zaf::ListSelectionMode ListSelectionManager::SelectionMode() const noexcept {
    return selection_mode_;
}


void ListSelectionManager::SetSelectionMode(zaf::ListSelectionMode mode) {

    selection_mode_ = mode;
    ReviseSelectionBySelectionMode();
    ResetSelectionStrategy();
}


void ListSelectionManager::ReviseSelectionBySelectionMode() {

    if (selection_mode_ == ListSelectionMode::None) {
        UnselectAllItems();
    }
    else if (selection_mode_ == ListSelectionMode::Single) {

        auto& selection_store = Parts().SelectionStore();
        if (selection_store.GetAllSelectedCount() <= 1) {
            return;
        }

        auto first_selected_index = selection_store.GetFirstSelectedIndex();
        if (!first_selected_index) {
            return;
        }

        selection_store.ReplaceSelection(*first_selected_index, 1);
    }
}


void ListSelectionManager::ResetSelectionStrategy() {

    selection_strategy_ = [this]() -> std::unique_ptr<ListSelectionStrategy> {

        auto context = &Parts();

        switch (selection_mode_) {
        case ListSelectionMode::Single:
            return std::make_unique<ListSingleSelectionStrategy>(context);

        case ListSelectionMode::SimpleMultiple:
            return std::make_unique<ListSimpleMultipleSelectionStrategy>(context);

        case ListSelectionMode::ExtendedMultiple:
            return std::make_unique<ListExtendedMultipleSelectionStrategy>(context);

        default:
            return std::make_unique<ListNoSelectionStrategy>(context);
        }
    }();
}


ListSelectionStrategy& ListSelectionManager::SelectionStrategy() const noexcept {
    return *selection_strategy_;
}


void ListSelectionManager::SelectAllItems() {

    auto data_source = Parts().Core().DataSource();
    if (!data_source) {
        return;
    }

    auto all_item_count = data_source->GetDataCount();

    auto& selection_store = Parts().SelectionStore();
    auto selected_count = selection_store.GetAllSelectedCount();
    if (all_item_count == selected_count) {
        return;
    }

    selection_store.ReplaceSelection(0, all_item_count);
}


void ListSelectionManager::UnselectAllItems() {

    auto& selection_store = Parts().SelectionStore();
    auto selected_count = selection_store.GetAllSelectedCount();
    if (selected_count == 0) {
        return;
    }

    selection_store.ReplaceSelection(0, 0);
}


void ListSelectionManager::SelectItemAtIndex(std::size_t index) {

    auto data_source = Parts().Core().DataSource();
    if (!data_source) {
        return;
    }

    ZAF_EXPECT(index < data_source->GetDataCount());

    auto& selection_store = Parts().SelectionStore();
    if (selection_store.IsIndexSelected(index)) {
        return;
    }

    switch (selection_mode_) {

    case ListSelectionMode::Single:
        selection_store.ReplaceSelection(index, 1);
        break;

    case ListSelectionMode::SimpleMultiple:
    case ListSelectionMode::ExtendedMultiple:
        selection_store.AddSelection(index, 1);
        break;

    default:
        break;
    }
}


void ListSelectionManager::UnselectItemAtIndex(std::size_t index) {

    auto data_source = Parts().Core().DataSource();
    if (!data_source) {
        return;
    }

    ZAF_EXPECT(index < data_source->GetDataCount());

    auto& selection_store = Parts().SelectionStore();
    if (!selection_store.IsIndexSelected(index)) {
        return;
    }

    selection_store.RemoveSelection(index, 1);
}

}